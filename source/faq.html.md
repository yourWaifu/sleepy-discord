---
title: Frequently asked questions - Sleepy Discord

language_tabs:

toc_footers:
  - <a href='https://github.com/tripit/slate'>Documentation Powered by Slate</a>

includes:

search: true
---

# FAQ

## Why isn't a channel for this library on the Discord API server?
Before a library gets a channel on the Discord API server, it must go though a moderator that'll review. The library has already been submitted to a moderator, and is currently waiting for detailed feedback for the library before getting a channel.

## Is this feature complete?
There are a few features not support just yet. They are createServer, editServer, editMember, editRole, pruneCount, editServerEmbed, editCurrentUser, createGroupDirectMessageChannel, and voice. Everything else is supported. Each one isn't support mostly because I'm confused about the parameters needed to get them to work.

## Does it work on a normal Computer like Windows or Linux?
Yes. I plan on having support on a large number of devices. However, Linux is untested.

## What makes mine different then the other Discord C++ libraries?
Unlike other Discord libraries, Sleepy Discord can support a large number of HTTP and Websocket libraries. This is because by default, Sleepy Discord does not use an input or output, but instead it calls for them. This is needed for support on many devices. In summery, it should be able to support a large number of devices.

## Why are parts of the json parser in c?
This is because Sleepy Discord started out as a c library, and it was then never touched ever again.

## Why a custom json parser?
It's understandable why this question comes up, because "reinventing the wheel" is consider bad, and all because the Existing wheel have already been tested in lots of circumstances and situations. But please, stop asking "Use a real json parser", because that's not happening unless the custom json parser really needs to be replaced because of some feature that gets added and that would requite a rewrite of the whole thing to get working.

Json is a big part of the Discord api, everything Discord sends back is in json. As Discord only sends json in a current way, assumptions can be made that can be used to speed up the json parsing. I wouldn't say that it's faster or better then Rapid json, but it is fast. Plus the user wouldn't be directly using the json parser to use the library, so didn't seem to matter.