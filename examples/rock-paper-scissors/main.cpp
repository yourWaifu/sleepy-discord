#include "sleepy_discord/sleepy_discord.h"
#include <list>
#include <iostream>
#include <memory>
#include <mutex>

class GameClient : public SleepyDiscord::DiscordClient {
private:
	enum State {
		NA               = 0,
		GET_READY        = 1,	//Game is starting
		SHOOT            = 2,	//waiting for answer
		ENDED            = 3
	};
	struct Game {
		SleepyDiscord::User player;
		std::shared_ptr<State> state;
		SleepyDiscord::Timer endGameTimer;
		std::mutex mutex;

		Game(const SleepyDiscord::User& gamePlayer) :
			player(gamePlayer), state(std::make_shared<State>(NA)) {}
		//This is needed when we want to copy a game
		Game(const Game& game) : 
			player(game.player), state(game.state), endGameTimer(game.endGameTimer) {}
#ifdef _DEBUG
		~Game() {
				std::cout << "DUBUG: game removed\n";
		}
#endif
		
		//void run(std::function<void(Game*)> startGame) {
		//	thread = std::thread(startGame, this);
		//}
	};
	std::list<Game> games;

	enum Weapon {
		ROCK     = 0,
		PAPER    = 1,
		SCISSORS = 2
	};

	enum winState {
		WIN  = 1,
		TIE  = 0,
		LOSE = 2
	};
	/*
	ROCK     - SCISSORS = 0 - 2 = -2 -> (-2 + 3) % 3 = WIN
	PAPER    - SCISSORS = 1 - 2 = -1 -> (-1 + 3) % 3 = LOSE
	ROCK     - PAPER    = 0 - 1 = -1 -> (-1 + 3) % 3 = LOSE
	PAPER    - ROCK     = 1 - 0 =  1 -> ( 1 + 3) % 3 = WIN
	SCISSORS - PAPER    = 2 - 1 =  1 -> ( 1 + 3) % 3 = WIN
	SCISSORS - ROCK     = 2 - 0 =  2 -> ( 2 + 3) % 3 = LOSE
	*/
//#ifdef _DEBUG
	void onHeartbeat() {
		std::cout << "heartbeat sent\n";
	}
	void onHeartbeatAck() {
		std::cout << "heartbeat acknowledged\n";
	}
//#endif
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) {
		if (message.startsWith("test hello")) {
			games.push_back(Game(message.author));	//create a game put it on the end of the list of games
#ifdef _DEBUG
			std::cout << "DUBUG: New game created. Number of games: " << games.size() << '\n';
#endif
			std::list<Game>::iterator game = --(games.end());
			const std::lock_guard<std::mutex> lock(game->mutex);
			*game->state = GET_READY;
			SleepyDiscord::Message countMessage = sendMessage(message.channelID, "**ROCK!**");
			schedule([this, countMessage]() { 
				this->editMessage(countMessage, "**PAPER!**"); 
			}, 1000);
			schedule([this, countMessage]() {
				this->editMessage(countMessage, "**SCISSORS!**");
			}, 2000);
			schedule([this, game, countMessage]() {
				const std::lock_guard<std::mutex> lock(game->mutex);
				*game->state = SHOOT;
				this->editMessage(countMessage, "**SHOOT!**");
			}, 3000);
			game->endGameTimer = schedule([this, game, message]() {
				game->mutex.lock();
				if (*game->state != ENDED) {
					*game->state = ENDED;
					sendMessage(message.channelID, "You lose.\nWhen I say ``shoot``, you pick ``rock``, ``paper``, or ``scissors``");
				}
				game->mutex.unlock();
				this->games.erase(game);
			}, 4000);
			return;
		}

		if (games.empty()) return;

		//get the player's choice
		Weapon playerChoice;
		if      (message.content == "rock"     || message.content == ":fist:"       ) playerChoice = ROCK;
		else if (message.content == "paper"    || message.content == ":raised_hand:") playerChoice = PAPER;
		else if (message.content == "scissors" || message.content == ":v:"          ) playerChoice = SCISSORS;
		else return;	//go back if there's no choice was detected
		for (Game& game : games) {
			const std::lock_guard<std::mutex> lock(game.mutex);
			if (game.player == message.author) {
#ifdef _DEBUG
				std::cout << "DUBUG: Player detected " << game.player.username << " state: " << *game.state << '\n';
#endif
				switch (*game.state) {
				case SHOOT: {
					*game.state = ENDED; //game state is set to end here, so that "you lose" isn't sent after a weapon was picked
					Weapon botChoice = static_cast<Weapon>(rand() % 3);	//random number 0 to 2
					switch (botChoice) {
					case ROCK:     sendMessage(message.channelID, ":fist:");        break;
					case PAPER:    sendMessage(message.channelID, ":raised_hand:"); break;
					case SCISSORS: sendMessage(message.channelID, ":v:");           break;
					}
					switch ((playerChoice - botChoice + 3) % 3) {
					case LOSE: sendMessage(message.channelID, ":smiley: I won, and you lost!");  break;
					case TIE:  sendMessage(message.channelID, ":neutral_face: It's a tie");      break;
					case WIN:  sendMessage(message.channelID, ":frowning: I lost, and you won"); break;
					}
				} break;
				case GET_READY: sendMessage(message.channelID, "Not yet, I didn't get to say ``shoot``."); break;
				default: sendMessage(message.channelID, "Error: unknown state"); break;
				}
			}
		}
	}
};

int main() {
	srand(static_cast<unsigned int>(time(0)));	//create seed to make random numbers
	GameClient client("Your Token Goes Here", SleepyDiscord::USER_CONTROLED_THREADS);
	client.setIntents(
		SleepyDiscord::Intent::SERVER_MESSAGES,
		SleepyDiscord::Intent::DIRECT_MESSAGES);
	client.run();
}