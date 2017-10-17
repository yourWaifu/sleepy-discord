#include "discord_object_interface.h"
#include "client.h"

namespace SleepyDiscord {
	//DiscordObject::DiscordObject(BaseDiscordClient* _client) {	//I'm deciding weather I should add this or not
	//	client = _client;
	//}

	BaseDiscordClient * DiscordObject::getOriginClient() {
		return BaseDiscordClient::clients[originClientHandle];
	}

	//void DiscordObject::changeDiscordClient(BaseDiscordClient& _client) {
	//	client = &_client;
	//}
}