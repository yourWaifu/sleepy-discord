#pragma once
#include <memory>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>

#include "sleepy_discord/net_endian.h"
#include "version_helper.h"

#include "asio_include.h"
#if defined(SLEEPY_USE_BOOST_ASIO)
#include <boost/asio/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#else
#include <asio/ssl.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#endif

#include <openssl/x509.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

// to do: use a log event and remove iostream include

namespace SleepyDiscord {
	class ASIOWebSocketConnection : public std::enable_shared_from_this<ASIOWebSocketConnection> {
	public:
		struct EventReceiver {
			std::function<void(std::vector<uint8_t> data)> onMessage;
			template<typename OnMessageFunc>
			void setOnMessage(OnMessageFunc callback) { onMessage = callback; }
			std::function<void(uint16_t code, std::string reason)> onClose;
			template<typename OnCloseFunc>
			void setOnClose(OnCloseFunc callback) { onClose = callback; }
			std::function<void()> onOpen;
			template<typename OnOpenFunc>
			void setOnOpen(OnOpenFunc callback) { onOpen = callback; }
		};

		ASIOWebSocketConnection(EventReceiver _receiver) :
			receiver(_receiver),
			sslContext(asio::ssl::context::tlsv13_client) // configure SSL, this object should outlive the socket for memory safty
		{}
		// the owner of the io context will be somewhere else, either that be the user's code or at the
		// beginning of the library's run function. Connect is called at the beginning of the object's life
		// and requires a io context. The other functions can't work while disconnected, so this pointer
		// is unlikly to be unvalid.
		std::shared_ptr<asio::io_context> ioContext;

		EventReceiver receiver;

		using SocketPtrType = std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>>;
		using MallocDeleter = decltype(&free);

		// https://stackoverflow.com/a/60580965
		// a few changes made to make this more sane. Firstly, we need to know the length later, so a pair is returned.
		// if error, 2nd in pair will be -1
		// Example:
		// auto secKey = encodeBase64(nonce.data(), nonce.size());
		// if (secKey.second == -1) return;
		std::pair<std::unique_ptr<char, MallocDeleter>, std::size_t> encodeBase64(const unsigned char* input, std::size_t length) {
			const auto pl = 4 * ((length + 2) / 3);
			auto output = reinterpret_cast<char*>(calloc(pl + 1, 1)); // +1 for the terminating null that EVP_EncodeBlock adds on
			const auto ol = EVP_EncodeBlock(reinterpret_cast<unsigned char*>(output), input, static_cast<int>(length));
			if (pl != ol) { std::cerr << "encode predicted " << pl << " but we got " << ol << "\n"; }
			return { std::unique_ptr<char, MallocDeleter>(output, free), ol};
		}

		// this check doesn't seem to handle padding so the check is replaced
		std::pair<std::unique_ptr<unsigned char, MallocDeleter>, std::size_t> decodeBase64(const char* input, std::size_t length) {
			const auto pl = 3 * length / 4;
			auto output = reinterpret_cast<unsigned char*>(calloc(pl + 1, 1));
			const auto ol = EVP_DecodeBlock(output, reinterpret_cast<const unsigned char*>(input), static_cast<int>(length));
			if (ol == -1) { std::cerr << "decode returned error \n"; }
			return { std::unique_ptr<unsigned char, MallocDeleter>(output, free), ol };
		}

		template<typename Task>
		std::shared_ptr<asio::steady_timer> makeTimer(std::chrono::duration<double> timeLeft, Task task) {
			auto timer = std::make_shared<asio::steady_timer>(*ioContext);
			timer->expires_after(std::chrono::duration_cast<asio::steady_timer::duration>(timeLeft));
			timer->async_wait(task);
			return timer;
		}

		void logError(std::string context, const asio::error_code& ec) {
			std::cerr << context;
			if (ec)
				std::cerr << ": " << ec.message() << " (Code: " << ec.value() << ")\n";
			else
				std::cerr << "\n";
		}

		void connect(const std::string& uri, std::shared_ptr<asio::io_context> _ioContext) {
			using namespace asio::ip;
			ioContext = _ioContext;
			if (!setup)
				init();
			if (closeRequestPtr)
				closeRequestPtr.reset();

			socketPtr = std::make_shared<asio::ssl::stream<tcp::socket>>(*ioContext, sslContext);

			auto self = shared_from_this(); // safely share this in async callbacks

			// If this takes too long, there's a good chance something went wrong but there was no error
			// So, we should cancel once it taking too long. How long this should wait depends, but 
			// 1.2 seconds sounds reasonable.
			auto cancelSignal = std::make_shared<asio::cancellation_signal>();
			using TimeoutTimerType = std::shared_ptr<asio::steady_timer>;
			TimeoutTimerType connectDeadline = makeTimer(std::chrono::duration<double>(1.2), [cancelSignal, self](const asio::error_code& err) {
				if (err)
					return;
					
				cancelSignal->emit(asio::cancellation_type::all);
				self->logError("connect took too long, canceling", err);
				self->socketPtr->async_shutdown([](const asio::error_code& _) {});
			});

			auto cleanUp = [self, connectDeadline]() {
				self->socketPtr->async_shutdown([](const asio::error_code& _) {});
				connectDeadline->cancel();
			};

			// extract infomation from uri
			// we don't need to handle edge cases because Discord's URI is very simular
			std::shared_ptr<std::string> hostname = std::make_shared<std::string>();
			std::shared_ptr<std::string> path = std::make_shared<std::string>();
			std::string protocol = uri.substr(0, uri.find("://"));
			std::string service = "443";
			size_t offset = protocol.length() + 3; // 3 is the length of ://
			if (offset < uri.length()) { // uri has more then a protocol
				(*hostname) = uri.substr(offset, uri.find("/", offset) - offset);
				offset += hostname->length();
				if (uri[offset] == '/') // url has a path
					(*path) = uri.substr(offset);
			}

			// get addresses
			std::shared_ptr<asio::ip::tcp::resolver> resolver = std::make_shared<asio::ip::tcp::resolver>(*ioContext);
			resolver->async_resolve(*hostname, service, [self, connectDeadline, cancelSignal, cleanUp, path, hostname, resolver](const asio::error_code& err, tcp::resolver::results_type results) {
				if (err) {
					self->logError("Couldn't connect, failed to resolve name query", err);
					return cleanUp();
				}

				// loop through results until one of them works
				asio::async_connect(self->socketPtr->next_layer(), results.begin(), results.end(), asio::bind_cancellation_slot(cancelSignal->slot(), [self, connectDeadline, cancelSignal, cleanUp, path, hostname](const asio::error_code& err, tcp::resolver::results_type::iterator foundEndpoint) {
					if (err) {
						self->logError("Failed to connect socket", err);
						return cleanUp();
					}
					// CONNECTED
					self->endpoint = *foundEndpoint;

					// set up TLS
					self->socketPtr->async_handshake(asio::ssl::stream_base::client, asio::bind_cancellation_slot(cancelSignal->slot(), [self, connectDeadline, cancelSignal, cleanUp, path, hostname](const asio::error_code& err) {
						if (err) {
							self->logError("Couldn't connect, SSL handshake failure", err);
							return cleanUp();
						}
						// HANDSHAKED

						// create websocket key
						// we'll need this to ensure that the connection is unique, and not preexisting
						// as such, the key needs to also be different for each connection
						std::array<unsigned char, 16> nonce = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
						if (RAND_bytes(nonce.data(), static_cast<int>(nonce.size())) != 1) {
							self->logError("Couldn't connect, random number generator failed", err);
							return cleanUp();
						}
						auto secKey = self->encodeBase64(nonce.data(), nonce.size());
						auto secKeyPtr = std::shared_ptr<char>(std::move(secKey.first));
						if (secKey.second == -1) return;
						std::size_t secKeyLength = secKey.second;

						// create the HTTP request for websockets connect
						std::string request; // I'm aware that it's better to call .reserve() here, but I'm lazy
						// GET /?v=9 HTTP/1.1
						request += "GET "; request += *path; request += " HTTP/1.1\r\n";
						// Host: gateway.discord.gg
						request += "Host: "; request += *hostname; request += "\r\n";
						// User-Agent: DiscordBot (SleepyDiscord, v#)
						request += "User-Agent: "; request += userAgent; request += "\r\n";
						request += "Accept: */*\r\n";
						request += "Upgrade: websocket\r\n";
						request += "Connection: Upgrade\r\n";
						request += "Sec-WebSocket-Version: 13\r\n";
						request += "Sec-WebSocket-Key: "; request.append(secKeyPtr.get(), secKey.second); request += "\r\n";
						request += "\r\n"; // header end

						// send the request
						auto requestPtr = std::make_shared<std::string>(std::move(request));
						auto messageBuffer = asio::buffer(requestPtr->data(), requestPtr->size());
						self->socketPtr->async_write_some(messageBuffer, asio::bind_cancellation_slot(cancelSignal->slot(), [requestPtr, secKeyPtr, secKeyLength, self, connectDeadline, cancelSignal, cleanUp](const asio::error_code& err, const std::size_t _) {
							if (err) {
								self->logError("failed to connect, Couldn't write request", err);
								return cleanUp();
							}
							// SENT
							// get the response
							//std::shared_ptr<asio::streambuf> readBuffer = std::make_shared<asio::streambuf>();
							auto responsePtr = std::make_shared<std::array<char, 256>>();
							auto readBuffer = asio::buffer(responsePtr->data(), responsePtr->size());
							self->socketPtr->async_read_some(readBuffer, asio::bind_cancellation_slot(cancelSignal->slot(), [responsePtr, secKeyPtr, secKeyLength, self, connectDeadline, cancelSignal, cleanUp](const asio::error_code& err, const std::size_t length) {
								if (err) {
									self->logError("failed to connect, Couldn't read response", err);
									return cleanUp();
								}
								// read and check that the response is correct
								std::string response{ responsePtr->data(), length };
								std::size_t statusCodeStart = response.find(' ') + 1;
								std::size_t statusCodeEnd = response.find(' ', statusCodeStart);
								std::string statusCode = response.substr(statusCodeStart, statusCodeEnd - statusCodeStart);
								std::size_t firstLineEnd = response.find("\r\n");
								std::size_t headerStart = firstLineEnd + 2; // + 2 to skip the new line
								// if the code isn't to switch, we should do redirections, but that's not implemented and we can add that later
								if (statusCode != "101") {
									std::string errorStr = "HTTP protocol switch to websocket failed, response was ";
									errorStr += response.substr(0, firstLineEnd);
									self->logError(std::move(errorStr), err);
									return cleanUp();
								}

								std::map<std::string, std::string> headers;
								std::size_t offset = headerStart;
								for (std::size_t newLinePos = response.find("\r\n", offset); // iterate headers
									newLinePos != offset && offset < length && newLinePos != -1;	// if not end of header
									newLinePos = response.find("\r\n", offset)
								) {
									std::string headerFieldLeft = response.substr(offset, response.find(':', offset) - offset);
									offset += headerFieldLeft.length();
									do { offset += 1; } while (offset < length && response[offset] == ' '); //ignore whitespace
									std::string headerFieldRight = response.substr(offset, newLinePos - offset);
									offset = newLinePos + 2;
									// insert to the map with the key being lowercase ASCII only
									std::transform(headerFieldLeft.begin(), headerFieldLeft.end(), headerFieldLeft.begin(), [](char c) { 
										return std::tolower(c);
									});
									headers.insert(std::make_pair(headerFieldLeft, std::move(headerFieldRight)));
								}
								offset += 2; // + 2 to skip end of header

								// easyer to write it like this, but a map of functions might be faster
								{
									auto iter = headers.find("upgrade");
									if (iter == headers.end()) {
										self->logError("HTTP protocol switch to websocket failed, Upgrade header from server not found", err);
										return cleanUp();
									}
									const std::string expectedValue = "websocket";
									std::string& value = iter->second;
									std::transform(value.begin(), value.end(), value.begin(), [](char c) {
										return std::tolower(c);
									});
									if (value != expectedValue) {
										self->logError("HTTP protocol switch to websocket failed, incorrect Upgrade value from server", err);
										return cleanUp();
									}
								}

								{
									auto iter = headers.find("connection");
									if (iter == headers.end()) {
										self->logError("HTTP protocol switch to websocket failed, Connection header from server not found", err);
										return cleanUp();
									}
									const std::string expectedValue = "upgrade";
									std::string& value = iter->second;
									std::transform(value.begin(), value.end(), value.begin(), [](char c) {
										return std::tolower(c);
									});
									if (value != expectedValue) {
										self->logError("HTTP protocol switch to websocket failed, incorrect Connection value from server", err);
										return cleanUp();
									}
								}

								const std::string websocketGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
								std::vector<unsigned char> hashInput{  }; // we have to use vector becuase string.data() in C++11 is const
								hashInput.resize(secKeyLength + websocketGUID.length()); // we seperate the count from the constructor with resize to stop compilers from being confused
								std::memcpy(hashInput.data(), secKeyPtr.get(), secKeyLength); // this looks dumb, but this avoids compilers errors ratelated to uchar and char conversions
								std::memcpy(hashInput.data() + secKeyLength, websocketGUID.data(), websocketGUID.length());
								std::array<unsigned char, SHA_DIGEST_LENGTH> acceptHash{ };
								SHA1(hashInput.data(), hashInput.size(), acceptHash.data());
								auto expectedAccept = self->encodeBase64(acceptHash.data(), acceptHash.size());
								auto expectedAcceptPtr = std::move(expectedAccept.first);
								if (expectedAccept.second == -1) {
									self->logError("Couldn't parse upgrade response, encode base64 failed", err);
									return cleanUp();
								}
								{
									auto iter = headers.find("sec-websocket-accept");
									if (iter == headers.end()) {
										self->logError("HTTP protocol switch to websocket failed, Sec-WebSocket-Accept header from server not found", err);
										return cleanUp();
									}
									std::string& value = iter->second;
									if (value.compare(0, value.length(), expectedAcceptPtr.get(), expectedAccept.second) != 0) {
										self->logError("HTTP protocol switch to websocket failed, incorrect Connection value from server", err);
										return cleanUp();
									}
								}

								{
									auto iter = headers.find("sec-websocket-extensions");
									const std::string expectedValue = "";
									if (iter != headers.end() && iter->second != expectedValue) {
										self->logError("HTTP protocol switch to websocket failed, unexpected Sec-WebSocket-Extensions header from server", err);
										return cleanUp();
									}
								}

								{
									auto iter = headers.find("sec-websocket-protocol");
									const std::string expectedValue = "";
									if (iter != headers.end() && iter->second != expectedValue) {
										self->logError("HTTP protocol switch to websocket failed, unexpected Sec-WebSocket-Protocol header from server", err);
										return cleanUp();
									}
								}

								// Receviced valid protocol switch
								// switch
								self->ready = true;
								connectDeadline->cancel();
								self->receiver.onOpen();

								// we don't need to read the rest of the buffer but it's in the way
								// read the whole response from the start while looking for the end
								self->receiveFullHandshake(responsePtr, length);
							}));
						}));
					}));
				}));
			});
		}

		template<
			typename BufferSource // The type of the source of the buffer pointer
		>
		void send(
			const BufferSource payload, // can be a vector of bytes or a string, so a template to handle both
			// our client only sends either binary or text, text is UTF-8, but we skip the validation,
			// so they are basically the same to us. But, the server might do the validation, so this
			// should be set correctly by the sender to prevent a invalid UTF-8 error.
			uint16_t opCode,
			std::function<void(const asio::error_code&)> callback
		) {
			if (!ready.load()) { // can't send while not connected
				std::cerr << "Can't send while not connected\n";
				return;
			}
			switch (opCode) {
			case 0x2: /*Binary*/ case 0x1: /*Text*/ case 0xA: /*Pong*/ case 0x8: /*Close*/
				break;
			default: 
				std::cerr << "Only sending text, binary, pong, and close is supported in our websocket connection \n";
				return;
				break;
			}
			
			bool isFin = true; // framgmentation isn't support, so this is always true
			uint8_t firstByte = (isFin << 7) | static_cast<uint8_t>(opCode);
			// the standard says that clients can only send masked messages, so hasMask is always true.
			// however, you might noticed that we don't support masking. this is because a mask of 0 is like having no mask.
			// so umm, I don't know
			bool hasMask = true;
			auto payloadLength = payload.size();
			uint8_t shortPayloadLength = payloadLength < 0b0111'1110 ? static_cast<uint8_t>(payloadLength) : payloadLength <= 0xFFFF ? static_cast<uint8_t>(126) : static_cast<uint8_t>(127); // static_cast to fix compiler getting confused about int literal sizes
			uint8_t secondByte = (static_cast<uint8_t>(hasMask) << 7) | shortPayloadLength;
			const std::array<uint8_t, 2> shortMessageHeader{ {firstByte, secondByte} };
			auto message = std::make_shared<std::basic_string<uint8_t>>(shortMessageHeader.data(), shortMessageHeader.size());
			// I know that I should use reserve here, but I'm lazy

			switch (shortPayloadLength) {
			case 126: {
				auto netInt16 = system2net16<uint8_t>(static_cast<uint16_t>(payloadLength));
				message->append(netInt16.data(), netInt16.size());
			} break;
			case 127: {
				auto netInt64 = system2net64<uint8_t>(static_cast<uint64_t>(payloadLength));
				message->append(netInt64.data(), netInt64.size());
			} break;
			default: break;
			}

			std::array<uint8_t, 4> maskingKey{ {0, 0, 0, 0} }; // No mask
			message->append(maskingKey.data(), maskingKey.size());

			// the standard says to mask the payload here, but we use mask of all zero
			// so, it doesn't make a difference
			message->append(payload.begin(), payload.end());

			auto messageBuffer = asio::buffer(message->data(), message->length());
			asio::async_write(*socketPtr, messageBuffer, [message, callback](const asio::error_code& err, const std::size_t length) {
				if (err)
					std::cerr << "failed to write to websocket\n";
				if (callback != nullptr)
					callback(err);
				return;
			});
		}

		void close(uint16_t code, const std::string& reason, bool closeSocketAfterSend = false) {
			if (!ready.load()) { // already can't sent, likly send a close and just receviced the close message from server
				if (closeSocketAfterSend && socketPtr != nullptr) {
					shutdown();
				}
				return;
			}

			if (socketPtr == nullptr) return; // already disconnected
			auto self = shared_from_this();
			auto netCloseInt = system2net16<uint8_t>(code);
			std::basic_string<uint8_t> closePayload{ netCloseInt.data(), netCloseInt.size() };
			closePayload.append(reinterpret_cast<const uint8_t*>(reason.data()), reason.length());
			send(closePayload, 0x8 /*Close*/, [self, closeSocketAfterSend](const asio::error_code& err) {
				if (err || closeSocketAfterSend) {
					self->shutdown();
				}
				else {
					return; // sent but wait to receive close from server
				}
			});
			ready = false; // stop anymore sending
		}

		struct CloseRequest {
			std::shared_ptr<asio::steady_timer> timeout;
		};

		bool disconnect(uint16_t code, const std::string& reason) {
			// disconnect is usally called twice by the library, from the heartbeat and close, so disable when disconnecting
			if (!ready || closeRequestPtr) {
				return false;
			}

			// to cleanly disconnect, we need to wait for the stream to end. To do that, we
			// send a close message to the server and wait to recevice a mirrored close message from the server
			// we might not get the close from the server, so we might want to close it after a few seconds
			auto self = shared_from_this();
			closeRequestPtr = std::unique_ptr<CloseRequest>(new CloseRequest{ makeTimer(std::chrono::duration<double>(1.0), [self](const asio::error_code& err) {
				if (err) return;
				std::string emptryStr = "";
				self->close(0, emptryStr, true); // can't send but it should shutdown
			}) });
			close(code, reason, false);
			return true;
		}

	private:
		asio::ssl::context sslContext;
		asio::ip::tcp::endpoint endpoint;
		bool setup = false;
		std::atomic_bool ready = false;
		SocketPtrType socketPtr = nullptr;
		std::unique_ptr<CloseRequest> closeRequestPtr;

		void init() {
			setup = true;
		}

		void shutdown() {
			auto self = shared_from_this();
			socketPtr->async_shutdown([self](const asio::error_code& err) {
				self->socketPtr.reset();
			});
		}

		void receiveFullHandshake(std::shared_ptr<std::array<char, 256>> responsePtr, const std::size_t length, int score = 0) {
			const static std::array<char, 4> endOfHandshake = { '\r', '\n', '\r', '\n' };
			for (std::size_t i = 0; i < length; i += 1) {
				if (responsePtr->data()[i] == endOfHandshake[score]) {
					score += 1;
					if (score == 4) {
						return receive(); // ready to read websocket messages
					}
						continue;
				}
				score = 0;
			}

			auto self = shared_from_this();
			auto readBuffer = asio::buffer(responsePtr->data(), responsePtr->size());
			self->socketPtr->async_read_some(readBuffer, [self, responsePtr, score](const asio::error_code err, std::size_t length) {
				if (err) return;
				self->receiveFullHandshake(responsePtr, length, score);
			});
		}

		void receive(std::size_t min = 2, std::shared_ptr<asio::streambuf> prev = nullptr) {
			if (!ready.load()) return;
			using namespace asio::ip;
			auto self = shared_from_this();
			std::shared_ptr<asio::streambuf> readBuffer = prev ? prev : std::make_shared<asio::streambuf>(); // asio only supports char, and I only found this out very late
			asio::async_read(*socketPtr, *readBuffer, asio::transfer_at_least(min), [self, readBuffer](const asio::error_code& err, const std::size_t length) {
				if (err) {
					std::cerr << "failed to read\n";
					return; // to do clean up
				}
				self->onFirstFewBytes(readBuffer, length);
			});
		}

		void onFirstFewBytes(std::shared_ptr<asio::streambuf> readBuffer, const std::size_t length) {
			// read the frame
			constexpr auto smallestFrameSize = 2;
			// based on the spec, there shouldn't be a message with a length of 1 or 0
			assert(smallestFrameSize <= length);

			std::istream inputStream(readBuffer.get());
			uint8_t firstByte; { // because of reading from a char array from asio, we have to convert to uint8_t
				char temp;
				inputStream.read(&temp, 1);
				std::memcpy(&firstByte, &temp, 1);
			}
			int8_t opCode = firstByte & 0b1111;

			constexpr uint8_t isFinBit = static_cast<uint8_t>(0b1000'0000);
			bool isFin = (firstByte & isFinBit) == isFinBit;
			if (!isFin) { // not my problem for now
				std::cerr << "Websocket fragmentation isn't supported. Ask the dev to add it if this is a problem.\n";
				return;
			}

			uint8_t secondByte; {
				char temp;
				inputStream.read(&temp, 1);
				std::memcpy(&secondByte, &temp, 1);
			}

			constexpr int8_t hasMaskBit = static_cast<uint8_t>(0b1000'0000);
			bool hasMask = (secondByte & hasMaskBit) == hasMaskBit;
			const std::size_t maskLength = hasMask ? sizeof(uint32_t) : 0;
			if (hasMask) { // Servers shouldn't sent masked payloads
				std::cerr << "Recevied masked websocket message, Servers shouldn't send masked payloads\n";
				return;
			}

			constexpr int8_t hasExtendedLength = static_cast<uint8_t>(0b0111'1111);
			int8_t length7Bit = secondByte & hasExtendedLength;
			std::size_t lengthOfLength;
			switch (length7Bit) {
			case 126: lengthOfLength = sizeof(uint16_t); break;
			case 127: lengthOfLength = sizeof(uint64_t); break;
			default: lengthOfLength = 0; break;
			}

			std::size_t bytesTilPayload = lengthOfLength + maskLength;
			const std::size_t bytesLeft = length - smallestFrameSize;
			Frame frame = { isFin, opCode, hasMask, length7Bit, maskLength };

			if (bytesTilPayload <= bytesLeft) {
				// need to receive more
				onFrame(frame, readBuffer, length);
			}
			else {
				// this is very unlikly but could happen
				auto self = shared_from_this();
				auto prevLength = length;
				asio::async_read(*socketPtr, *readBuffer, asio::transfer_at_least(bytesTilPayload), [self, frame, readBuffer, prevLength](const asio::error_code& err, const std::size_t length) {
					if (err) {
						std::cerr << "failed to read frame\n";
						return; // to do clean up
					}
					self->onFrame(frame, readBuffer, prevLength + length);
				});
			}
		}

		struct Frame {
			bool isFin;
			int8_t opCode;
			bool hasMask;
			int8_t length7Bit;
			std::size_t maskLength;
		};

		void onFrame(Frame frame, std::shared_ptr<asio::streambuf> readBuffer, std::size_t length) {
			auto self = shared_from_this();
			std::istream inputStream(readBuffer.get());
			int extendedLengthPrefixLength;
			std::size_t payloadLength;
			switch (frame.length7Bit) {
			case 126: {
				constexpr std::size_t size = sizeof(uint16_t);
				extendedLengthPrefixLength = size;
				std::array<uint8_t, size> networkInt{}; {
					std::array<char, size> temp;
					inputStream.read(temp.data(), networkInt.size());
					std::memcpy(networkInt.data(), temp.data(), networkInt.size());
				}
				payloadLength = net2System16(networkInt);
			} break;
			case 127: {
				constexpr std::size_t size = sizeof(uint64_t);
				extendedLengthPrefixLength = size;
				std::array<uint8_t, size> networkInt{}; {
					std::array<char, size> temp;
					inputStream.read(temp.data(), networkInt.size());
					std::memcpy(networkInt.data(), temp.data(), networkInt.size());
				}
				uint64_t temp = net2System64(networkInt);

				constexpr auto pointerMax = (std::numeric_limits<std::size_t>::max)();
				constexpr bool pointerIsTooSmall = pointerMax < (std::numeric_limits<uint64_t>::max)();
				if ( // on a 32 bit or 16 bit system, we can't put the message in memory
					// the compiler should be smart enough to get rid of this on 64-bit systems
					pointerIsTooSmall && (pointerMax < payloadLength)
					) { // doubt that Discord would ever sent a 4 GB websocket payload, so just kill it
					std::cerr << "Can't read message larger then max pointer value\n";
					disconnect(1009, "sizeof(PTR)<MSG");
					return;
				}
				
				payloadLength = static_cast<std::size_t>(temp);
			} break;
			default:
				extendedLengthPrefixLength = 0;
				payloadLength = frame.length7Bit;
				break;
			}

			const std::size_t maskStart = 2 + static_cast<size_t>(extendedLengthPrefixLength);
			const std::size_t payloadStart = maskStart + frame.maskLength;
			const std::size_t leftOverLength = length - payloadStart;

			if (leftOverLength < payloadLength) {
				// we don't have the whole message, loop receive until we have the whole message
				const std::size_t bytesLeftToGet = payloadLength - leftOverLength;
				asio::async_read(*(self->socketPtr), *readBuffer, asio::transfer_at_least(bytesLeftToGet), [self, readBuffer, frame, payloadLength, bytesLeftToGet](const asio::error_code& err, const std::size_t length) {
					if (err) {
						std::cerr << "failed to read whole\n";
						return; // to do clean up
					}
					self->onPayload(frame.opCode, readBuffer, payloadLength);
					self->afterPayload(length - bytesLeftToGet, readBuffer);
					});
			}
			else {
				// we have the whole message, we don't need to read again
				self->onPayload(frame.opCode, readBuffer, payloadLength);
				afterPayload(leftOverLength - payloadLength, readBuffer);
			}
		}

		void onPayload(int8_t opCode, std::shared_ptr<asio::streambuf> readBuffer, std::size_t payloadLength) {
			std::vector<uint8_t> payload{}; payload.resize(payloadLength); // compiler keeps thinking length is a char array
			asio::buffer_copy(asio::buffer(payload), readBuffer->data());
			readPayload(opCode, std::move(payload));
			readBuffer->consume(payloadLength);
		}

		void readPayload(int8_t opCode, std::vector<uint8_t> payload) {
			constexpr int8_t continueOp = 0x0; constexpr int8_t textOp = 0x1; constexpr int8_t binaryOp = 0x2;
			constexpr int8_t closeOp = 0x8; constexpr int8_t pingOp = 0x9; constexpr int8_t pongOp = 0xA;

			switch (opCode) {
				// the only difference between text and binary is that text should be valid utf-8
				// I believe utf-8 verification is unnecessary, so the two are basically the same.
			case textOp: case binaryOp: {
				// copy payload and share it with the receiver
				receiver.onMessage(payload);
			} break;
			case closeOp: {
				// first two bytes of the payload is the close code
				std::array<uint8_t, sizeof(uint16_t)> networkInt{};
				std::memcpy(networkInt.data(), payload.data(), networkInt.size());
				uint16_t closeCode = net2System16(networkInt);
				auto reasonStart = payload.begin();
				std::advance(reasonStart, networkInt.size());
				std::string reason = std::string{reasonStart, payload.end()};
				std::cout << "Close " << closeCode << ": " << reason << '\n';
				receiver.onClose(closeCode, reason); // tell the receiver before closing
				close(closeCode, reason, true); // echo close or close socket, can be both
				ready = false; // ready should already be false here but just in case
				if (closeRequestPtr) { // the close request timeout is no longer needed
					closeRequestPtr->timeout->cancel();
				}
			} break;
			case pingOp:
				send(payload, static_cast<uint16_t>(pongOp), nullptr);
				break;
			case pongOp:
				// OK
				break;
			case continueOp:
				// shouldn't happen because we don't support websocket fragmentation
			default:
				std::cerr << "Unknown op code from server\n";
				return;
			}
		}

		void afterPayload(std::size_t notProcessedLength, std::shared_ptr<asio::streambuf> readBuffer) {
			if (notProcessedLength == 0) {
				return receive(); // continue recevie loop
			}
			else if (notProcessedLength < 2) {
				return receive(2 - 1, readBuffer); // continue recevie loop but keep buffer
			}
			else {
				// we have enough data to read a frame
				return onFirstFewBytes(readBuffer, notProcessedLength);
			}
		}
	};

	typedef std::weak_ptr<ASIOWebSocketConnection> WebsocketConnection;
}