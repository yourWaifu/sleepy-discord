#pragma once

/** Async Curl Wrapper for ASIO C++ **/

// this code doesn't work still

#include <curl/multi.h>
#include <chrono>
#include <memory>
#include <utility>
#include <list>
#include "sleepy_discord/asio_include.h"
#include "sleepy_discord/asio_schedule.h"
#include <asio/steady_timer.hpp>
#include "sleepy_discord/http.h"

namespace SleepyDiscord {
	class CurlHTTPGlobal { // RAII global init and cleanup
	public:
		CurlHTTPGlobal(const CurlHTTPGlobal&) = delete;
		CurlHTTPGlobal(CurlHTTPGlobal&&) = delete;
		CurlHTTPGlobal& operator= (const CurlHTTPGlobal&) = delete;
		CurlHTTPGlobal& operator= (CurlHTTPGlobal&) = delete;

		// Users that already use curl, should replace their curl_global_init with this so that we don't kill their curl
		static std::shared_ptr<CurlHTTPGlobal> init(long flags = CURL_GLOBAL_ALL) {
			initImpl<long>(flags);
		}

		// Users that already use curl_global_init but can't replace it, should use this
		static std::shared_ptr<CurlHTTPGlobal> getExist() {
			return initImpl<std::nullptr>(std::nullptr);
		}

	private:
		CurlHTTPGlobal(std::nullptr) {}
		CurlHTTPGlobal() : CurlHTTPGlobal(CURL_GLOBAL_ALL) {}
		CurlHTTPGlobal(long flags) {
			auto result = curl_global_init(flags);
			if (!result) throw easy_error(result);
		}
		~CurlHTTPGlobal() noexcept {
			curl_global_cleanup();
		}

		template<class ConstructorArg>
		static std::shared_ptr<CurlHTTPGlobal> initImpl(ConstructorArg flags) {
			if (std::shared_ptr<CurlHTTPGlobal> s = CurlHTTPGlobal::signalton.lock()) {
				return s;
			}
			else {
				auto ptr = std::make_shared<CurlHTTPGlobal>(flags);
				CurlHTTPGlobal::signalton = ptr;
				return ptr;
			}
		}

		static std::weak_ptr<CurlHTTPGlobal> signalton;
	};

	class CurlHTTPReceiver {
	public:
		void cancel() {
			curl_multi_remove_handle(clientPtr->getCurlM(), handle);
		}
		void setResponseCallback(const ResponseCallback& callback) {
			responseCallback = callback;
		}
#ifdef __cpp_coroutines

#endif
		void runResponseCallback(CURL* easyHandle) {

		}

		void open(curl_socket_t& socket) {
			state = State::Open;
		}

		const State getState() {
			return state;
		}

		enum class State : char {
			None = 0,
			Open = 1,
			Closed = 2,
		};

		void close() {
			state = State::Closed;
		}

		std::atomic_bool isReading = false;
		std::atomic_bool isWriting = false;

	private:
		std::shared_ptr<CurlHTTPASIOClient> clientPtr;
		std::shared_ptr<CURL> handle;
		std::atomic_char state = State::None;
		ResponseCallback responseCallback;
	};

	class CurlHTTPASIOClient : std::enable_shared_from_this<CurlHTTPASIOClient> {
		struct PrivateProps { explicit PrivateProps() = default; std::shared_ptr<asio::io_context> _ioContext; };
	public:
		// Curl is going to need a pointer to self, so a seperate function is needed where self is valid
		// Call CurlHTTPASIOClient::create instead of this
		CurlHTTPASIOClient(PrivateProps props) :
			curlMPtr(curl_multi_init()),
			curlGPtr(CurlHTTPGlobal::init(CURL_GLOBAL_ALL)),
			ioContext(props._ioContext),
			timeoutTimer(*props._ioContext)
		{}

		static std::shared_ptr<CurlHTTPASIOClient> create(std::shared_ptr<asio::io_context> _ioContext) {
			auto self = std::make_shared<CurlHTTPASIOClient>(PrivateProps{ _ioContext });
			curl_multi_setopt(curlMPtr.get(), CURLMOPT_SOCKETFUNCTION, &CurlHTTPASIOClient::socketCallback);
			curl_multi_setopt(curlMPtr.get(), CURLMOPT_SOCKETDATA, self.get());
			curl_multi_setopt(curlMPtr.get(), CURLMOPT_TIMERFUNCTION, &CurlHTTPASIOClient::timeoutCallback);
			curl_multi_setopt(curlMPtr.get(), CURLMOPT_TIMERDATA, self.get());

			return self;
		}

		int checkSocketActivity(CURL* easyHandle, curl_socket_t& socket, int& action, void*) {
			std::list<std::shared_ptr<CurlHTTPReceiver>>::iterator* iteratorPtr;
			curl_easy_getinfo(easyHandle, CURLINFO_PRIVATE, iteratorPtr);
			std::shared_ptr<CurlHTTPReceiver> receiverPtr = (*iteratorPtr);

			int events = 0;
			switch (action) {
			case CURL_POLL_IN:
			case CURL_POLL_OUT:
			case CURL_POLL_INOUT: {
				switch (receiverPtr->getState()) {
				case CurlHTTPReceiver::State::Closed:
				default:
					// it's possible for socket to already be removed at this point
					return;
				case CurlHTTPReceiver::State::None:
					// start curl context
					iteratorPtr->start();
					curl_multi_assign(curlMPtr.get(), socket, reinterpret_cast<void*>(receiverPtr.get()));
					break;
				case CurlHTTPReceiver::State::Open:
					break;
				}

				// start polling
				if (/*read*/ action & CURL_POLL_IN) read();
				if (/*write*/ action & CURL_POLL_OUT) write();

			} break;

			case CURL_POLL_REMOVE:
				// end polling
				// note socket is invalid here
				receiverPtr->close();
				break;
			default:
				break;
			}
			return 0;
		}

		void read() {
			// read socket
		}

		void write() {
			// write socket
		}

		void checkMultiStatus() {
			char* doneURL;
			CURLMsg* message;
			int pending;

			while (true) {
				message = curl_multi_info_read(curlMPtr.get(), &pending);
				if (!message) break;
				switch (message->msg) {
				case CURLMSG_DONE: {
					easyHandle = message->easy_handle;
					std::list<std::shared_ptr<CurlHTTPReceiver>>::iterator* iteratorPtr;
					curl_easy_getinfo(easyHandle, CURLINFO_PRIVATE, iteratorPtr);
					(*iteratorPtr)->get()->runResponseCallback(easyHandle);
					
					curl_multi_remove_handle(curlMPtr.get(), easy_handle);
					curl_easy_cleanup(easy_handle);
				} break;
				default: break;
				}
			}
		}

		static int socketCallback(CURL* easyHandle, curl_socket_t s, int what, CurlHTTPASIOClient* clientPtr, void* socketPtr) {
			return clientPtr->checkSocketActivity(easyHandle, s, what, socketPtr);
		}

		int updateTimeoutTimer(CURLM* multi, long timeoutMS) {
			if (timoutMS < 0) {
				timeoutTimer.cancel();
			}
			else {
				std::chrono::duration<double> timeout = timeoutMS == 0 ? std::chrono::duration<double>::zero() : static_cast<double>(timeout);
				timeoutTimer.expires_after(std::chrono::duration_cast<asio::steady_timer::duration>(timeout));
				auto self = shared_from_this();
				timeoutTimer.async_wait([self, multi](const asio::error_code& error) {
					if (err)
						return;

					int runningHandles;
					curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0, &runningHandles);
					checkMultiStatus();
				});
			}
			return 0;
		}

		static int timeoutCallback(CURLM* multi, long timeoutMS, CurlHTTPASIOClient* clientPtr) {
			return clientPtr->updateTimeoutTimer(multi, timeoutMS);
		}

		CURLM* getCurlM() {
			return curlMPtr.get();
		}

		std::list<std::shared_ptr<CurlHTTPReceiver>>::iterator makeWaiter(std::shared_ptr<CURL> handle) {
			waiterList.push_back(std::move(std::make_shared<CurlHTTPReceiver>(clientPtr, handle)));
			auto iterator = std::prev(waiterList.end());
			return iterator;
		}

	private:
		std::unique_ptr<CURLM, decltype(&curl_multi_cleanup)> curlMPtr;
		std::shared_ptr<CurlHTTPGlobal> curlGPtr;
		std::shared_ptr<asio::io_context> ioContext;
		asio::steady_timer timeoutTimer;
		int runningHandles;
		std::list<std::shared_ptr<CurlHTTPReceiver>> waiterList;
	};

	class CurlHTTPRequest {
	public:
		CurlHTTPRequest(std::shared_ptr<CurlHTTPASIOClient> _clientPtr) {
			clientPtr = _clientPtr;
			handle = std::shared_ptr<CURL>(curl_easy_init(), &curl_easy_cleanup);
		}
		void setMethod(RequestMethod method) {
			switch (method) {
			case RequestMethod::Post:   curl_easy_setopt(handle.get(), CURLOPT_POST, 1L); break;
			case RequestMethod::Patch:  curl_easy_setopt(handle.get(), CURLOPT_CUSTOMREQUEST, "PATCH"); break;
			case RequestMethod::Delete: curl_easy_setopt(handle.get(), CURLOPT_CUSTOMREQUEST, "DELETE"); break;
			case RequestMethod::Get:    curl_easy_setopt(handle.get(), CURLOPT_HTTPGET, 1L); break;
			case RequestMethod::Put:    curl_easy_setopt(handle.get(), CURLOPT_CUSTOMREQUEST, "PUT"); break;
			}
		}
		void setUrl(const std::string& url) {
			curl_easy_setopt(handle.get(), CURLOPT_URL, url.c_str(), url.length());
		}
		void setBody(const std::string& jsonParameters) {
			// Copies
			curl_easy_setopt(handle.get(), CURLOPT_POSTFIELDSIZE_LARGE,
				static_cast<curl_off_t>(jsonParameters.length()));
			curl_easy_setopt(handle.get(), CURLOPT_COPYPOSTFIELDS, jsonParameters.c_str());
		}
		void setHeader(const std::vector<HeaderPair>& header) {
			struct curl_slist* list = nullptr;
			for (auto item = header.cbegin(); item != header.cend(); ++item) {
				const std::string name = item->name; // name is a const char* for some reason
				std::size_t headerFieldLength = name.length();
				constexpr auto emptyValueStr = ";";
				constexpr std::size_t emptyValueLen = 1;
				constexpr auto nameValueSeperatorStr = ": ";
				constexpr std::size_t nameValueSeperatorLen = 2;
				if (item->value.empty()) {
					headerFieldLength += emptyValueLen;
				}
				else {
					headerFieldLength += nameValueSeperatorStrLen;
					headerFieldLength += item->value.length();
				}

				std::string field_str; // string copy
				field_str.reserve(headerFieldLength);
				field_str += name;
				if (item->value.empty()) {
					field_str.append(emptyValueStr, emptyValueLen);
				}
				else {
					field_str.append(nameValueSeperatorStr, nameValueSeperatorLen);
					field_str += item->value;
				}

				auto temp = curl_slist_append(list, field_str);
				if (temp) {
					list = temp;
				}
				curl_easy_setopt(handle.get(), CURLOPT_HTTPHEADER, list);
				headerListPtr.reset();
				headerListPtr = { list, &curl_slist_free_all };
			}
		}
		void setMultipart(const std::vector<Part>& parts) {
			struct curl_mime* mime = curl_mime_init(handle.get());

			for (auto& part : parts) {
				struct curl_mimepart* partPtr = curl_mime_addpart(mime);
				if (part.isFile) curl_mime_filedata(partPtr, part.value);
				else             curl_mime_data(partPtr, part.value.data(), part.value.length());
				curl_mime_name(partPtr, part.name.c_str());
			}
			curl_easy_setopt(handle.get(), CURLOPT_MIMEPOST, mime);
			mimePtr.reset();
			mimePtr = { mime, &curl_mime_free };
		}
		std::shared_ptr<CurlHTTPReceiver> send(const ResponseCallback& callback) {
			auto waiter = clientPtr->makeWaiter(handle);
			curl_easy_setopt(handle.get(), CURLOPT_PRIVATE, static_cast<void*>(&waiter)); // needed to get the our waiter later
			curl_multi_add_handle(clientPtr->getCurlM(), handle.get());
			int runningHandles;
			curl_multi_socket_action(clientPtr->getCurlM(), CURL_SOCKET_TIMEOUT, 0, runningHandles);
			return *waiter;
		}
	private:
		std::shared_ptr<CurlHTTPASIOClient> clientPtr;
		std::shared_ptr<CURL> handle;
		std::unique_ptr<struct curl_slist, decltype(&curl_slist_free_all)> headerListPtr;
		std::unique_ptr<struct curl_mime, decltype(&curl_mime_free)> mimePtr;
	};
}