---
title: Breaking changes &middot; Sleepy Discord

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---
[⮌ Go back to documentation](documentation.html)

#Breaking changes

Updating the library may stop your code from compiling correctly or behave differently. This is a list of changes that would cause those issues.

## Add Voice support [#76](https://github.com/yourWaifu/sleepy-discord/pull/76/files)
<!--First commit-->
 * Replaced ``DirectMessageChannel``and ``DMChannel`` with ``Channel`` objects with type set to ``DM`` enum
	 * Any thing that was the type ``DMChannel`` is now replace with ``Channel``
	 * This will allow you to be able to use DM channels on functions require a ``Chennel`` object
 * ``BaseDiscordClient::connect`` and ``BaseDiscordClient::disconnect`` now requires 2 new parameters, ``messageProcessor`` and ``connection``
	 * ``BaseDiscordClient::send`` also now requires a ``connection`` parameter
	 * This will allow you to have more then one connection for a Client, a feature needed for voice support
 * Some Discord Events use different parameter types instead of a ``std::string``
 	 * ``BaseDiscordClient::onReady`` now uses a ``Ready`` object
	 * ``BaseDiscordClient::onEditVoiceState`` now uses ``VoiceState``
	 * ``BaseDiscordClient::onEditVoiceServer`` now uses ````
 * ``SLEEPY_AUTO_SELECT_CLIENT`` has be removed and is now on by default when using ``#include "sleepy_discord/sleepy_discord.h"``
 * Using numbers in the Discord Client constructor is now deprecated
	* Instead use ``USER_CONTROLED_THREADS`` or ``USE_RUN_THREAD``