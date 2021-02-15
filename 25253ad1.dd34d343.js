(window.webpackJsonp=window.webpackJsonp||[]).push([[34],{104:function(e,n,t){"use strict";t.r(n),t.d(n,"frontMatter",(function(){return i})),t.d(n,"metadata",(function(){return s})),t.d(n,"toc",(function(){return c})),t.d(n,"default",(function(){return l}));var a=t(3),r=t(7),o=(t(0),t(321)),i={title:"include/sleepy_discord/sleepy_discord.h"},s={unversionedId:"reference/Files/sleepy__discord_8h",id:"reference/Files/sleepy__discord_8h",isDocsHomePage:!1,title:"include/sleepy_discord/sleepy_discord.h",description:"Namespaces",source:"@site/docs\\reference\\Files\\sleepy__discord_8h.md",slug:"/reference/Files/sleepy__discord_8h",permalink:"/sleepy-discord-docs/docs/reference/Files/sleepy__discord_8h",version:"current",sidebar:"Reference",previous:{title:"include/sleepy_discord/session.h",permalink:"/sleepy-discord-docs/docs/reference/Files/session_8h"},next:{title:"include/sleepy_discord/snowflake.h",permalink:"/sleepy-discord-docs/docs/reference/Files/snowflake_8h"}},c=[{value:"Namespaces",id:"namespaces",children:[]},{value:"Defines",id:"defines",children:[]},{value:"Macro Documentation",id:"macro-documentation",children:[{value:"define SLEEPY_DEFINE_CUSTOM_CLIENT",id:"define-sleepy_define_custom_client",children:[]}]},{value:"Source code",id:"source-code",children:[]}],d={toc:c};function l(e){var n=e.components,t=Object(r.a)(e,["components"]);return Object(o.b)("wrapper",Object(a.a)({},d,t,{components:n,mdxType:"MDXLayout"}),Object(o.b)("h2",{id:"namespaces"},"Namespaces"),Object(o.b)("table",null,Object(o.b)("thead",{parentName:"table"},Object(o.b)("tr",{parentName:"thead"},Object(o.b)("th",{parentName:"tr",align:null},"Name"))),Object(o.b)("tbody",{parentName:"table"},Object(o.b)("tr",{parentName:"tbody"},Object(o.b)("td",{parentName:"tr",align:null},Object(o.b)("strong",{parentName:"td"},Object(o.b)("a",{parentName:"strong",href:"/docs/reference/Namespaces/namespace_sleepy_discord"},"SleepyDiscord")))))),Object(o.b)("h2",{id:"defines"},"Defines"),Object(o.b)("table",null,Object(o.b)("thead",{parentName:"table"},Object(o.b)("tr",{parentName:"thead"},Object(o.b)("th",{parentName:"tr",align:null}),Object(o.b)("th",{parentName:"tr",align:null},"Name"))),Object(o.b)("tbody",{parentName:"table"},Object(o.b)("tr",{parentName:"tbody"},Object(o.b)("td",{parentName:"tr",align:null}),Object(o.b)("td",{parentName:"tr",align:null},Object(o.b)("strong",{parentName:"td"},Object(o.b)("a",{parentName:"strong",href:"/docs/reference/Files/sleepy__discord_8h#define-sleepy_define_custom_client"},"SLEEPY_DEFINE_CUSTOM_CLIENT")))))),Object(o.b)("h2",{id:"macro-documentation"},"Macro Documentation"),Object(o.b)("h3",{id:"define-sleepy_define_custom_client"},"define SLEEPY_DEFINE_CUSTOM_CLIENT"),Object(o.b)("pre",null,Object(o.b)("code",{parentName:"pre",className:"language-cpp"},"#define SLEEPY_DEFINE_CUSTOM_CLIENT namespace SleepyDiscord {\\\ntypedef BaseDiscordClient DiscordClient;\\\n}\n")),Object(o.b)("h2",{id:"source-code"},"Source code"),Object(o.b)("pre",null,Object(o.b)("code",{parentName:"pre",className:"language-cpp"},"#pragma once\n\n#define SLEEPY_DEFINE_CUSTOM_CLIENT \\\nnamespace SleepyDiscord {\\\ntypedef BaseDiscordClient DiscordClient;\\\n}\n\n#ifdef SLEEPY_CUSTOM_CLIENT\n    #include \"client.h\"\n    SLEEPY_DEFINE_CUSTOM_CLIENT\n#elif defined(SLEEPY_DISCORD_CMAKE)\n    #if defined(EXISTENT_WEBSOCKETPP)\n        #include \"websocketpp_websocket.h\"\n    #elif defined(EXISTENT_UWEBSOCKETS)\n        #include \"uwebsockets_websocket.h\"\n    #else\n        #include \"client.h\"\n        SLEEPY_DEFINE_CUSTOM_CLIENT\n    #endif\n#else\n    #include \"websocketpp_websocket.h\"\n    #ifdef NONEXISTENT_WEBSOCKETPP\n        #include \"uwebsockets_websocket.h\"\n        #ifdef NONEXISTENT_UWEBSOCKETS\n            #include \"client.h\"\n            SLEEPY_DEFINE_CUSTOM_CLIENT\n        #endif\n    #endif\n#endif\n\n/*\nFEEDBACK\nacdenisSK - Today at 8:23 PM\natleast make it parse the json in the function ffs\n\nThe Almighty Shubshub - Today at 8:25 PM\nWhy aren't the message_create events bundled with an isprivate identifier?\n\nDanny - Today at 10:29 PM\n1. sleepy_c++_discord_library.h is an awful library name, consider using something a bit more sane like sleepy_discord.hpp or something.\n2. C++ style guide is snake_case not ThisCase or thisCase.\n3. SleepyDiscord::init is a global function that is a C-ism. C++ we tend to use RAII.\n4. Your use of new myEventClass has no notion of ownership.\n5. Your parameter jsonMessage has no notion of ownership.\n6. Forcing your users to do the heartbeat themselves is poor design.\n7. Sleep is not a standard function. std::this_thread::sleep_for(std::chrono::seconds(1)) is though.\n8. heartbeat is a global function again which makes me wonder how your design is.\n9. Casting to std::string like (std::string)message.author.username is most definitely the wrong thing to do.\n\nbased on your code I don't even think you know what modern C++ is\nlook up C++11/14/17\n\nmeh\nownership woes still\nand then there's the responsibility of creating the message\nalso I don't like your message API\nstd::string already exists.\ne.g. message.content.find(\"whcg hello\") == 0 works too\nand you could then use message.content with whatever accepts an std::string\nfor all I know message.content already exists.\n\nit's pretty meh\nI don't want some parsed JSON object\nif you're gonna use boost.asio use it all the way through\nhttps://github.com/Rapptz/Gears/blob/docs/gears/string/predicate.hpp\n\nDigiTechs - Today at 12:17 AM\nThat's a pretty bad way of passing data. I guess if it's written in C it makes sense but you should have written a thin wrapper in C++ for it\n\nI see, you are manually constructing JSON payload too\nSleepy Flower Girl - Today at 2:28 AM\nyea\nI don't see that as problem yet\nqwename - Today at 2:29 AM\nWell, it's error-prone, and you might not know the intricacies of JSON enough\nTiltMeSenpai - Today at 2:29 AM\nthere is a json lib for cpp floating around\nyou want that?\nSleepy Flower Girl - Today at 2:29 AM\nI don't need a json lib\nqwename - Today at 2:30 AM\nThe least you can do is pack it into a helper function\nSleepy Flower Girl - Today at 2:30 AM\nI understand it enugh that I don't need a lib for that\nqwename - Today at 2:31 AM\nI pass a dictionary around until I actually need to send the data, then I call a library function to return a JSON payload\nThe point isn't whether you understand it or not(edited)\nTiltMeSenpai - Today at 2:31 AM\nok that's not horrible actually\nqwename - Today at 2:31 AM\nRight now you have to specify all the curly brackets and commas and whatnot(edited)\nNEW MESSAGES\nqwename - Today at 2:32 AM\nWhich takes away time and effort from what you are actually accomplishing\nSo I suggest the minimum of putting that in a helper method.\nI think the map class is an associative array that can do what you need\nFrom <map>\n\nzey - Today at 12:24 AM\ni mention someone if\n- they'll otherwise miss it\n- they're afk\n- it's really important\n- it's Laura(edited)\n\n*/\n\nnamespace SleepyDiscord {\n    \n}\n")),Object(o.b)("hr",null),Object(o.b)("p",null,"Updated on  9 February 2021 at 07:04:58 Eastern Standard Time"))}l.isMDXComponent=!0},321:function(e,n,t){"use strict";t.d(n,"a",(function(){return p})),t.d(n,"b",(function(){return b}));var a=t(0),r=t.n(a);function o(e,n,t){return n in e?Object.defineProperty(e,n,{value:t,enumerable:!0,configurable:!0,writable:!0}):e[n]=t,e}function i(e,n){var t=Object.keys(e);if(Object.getOwnPropertySymbols){var a=Object.getOwnPropertySymbols(e);n&&(a=a.filter((function(n){return Object.getOwnPropertyDescriptor(e,n).enumerable}))),t.push.apply(t,a)}return t}function s(e){for(var n=1;n<arguments.length;n++){var t=null!=arguments[n]?arguments[n]:{};n%2?i(Object(t),!0).forEach((function(n){o(e,n,t[n])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(t)):i(Object(t)).forEach((function(n){Object.defineProperty(e,n,Object.getOwnPropertyDescriptor(t,n))}))}return e}function c(e,n){if(null==e)return{};var t,a,r=function(e,n){if(null==e)return{};var t,a,r={},o=Object.keys(e);for(a=0;a<o.length;a++)t=o[a],n.indexOf(t)>=0||(r[t]=e[t]);return r}(e,n);if(Object.getOwnPropertySymbols){var o=Object.getOwnPropertySymbols(e);for(a=0;a<o.length;a++)t=o[a],n.indexOf(t)>=0||Object.prototype.propertyIsEnumerable.call(e,t)&&(r[t]=e[t])}return r}var d=r.a.createContext({}),l=function(e){var n=r.a.useContext(d),t=n;return e&&(t="function"==typeof e?e(n):s(s({},n),e)),t},p=function(e){var n=l(e.components);return r.a.createElement(d.Provider,{value:n},e.children)},u={inlineCode:"code",wrapper:function(e){var n=e.children;return r.a.createElement(r.a.Fragment,{},n)}},h=r.a.forwardRef((function(e,n){var t=e.components,a=e.mdxType,o=e.originalType,i=e.parentName,d=c(e,["components","mdxType","originalType","parentName"]),p=l(t),h=a,b=p["".concat(i,".").concat(h)]||p[h]||u[h]||o;return t?r.a.createElement(b,s(s({ref:n},d),{},{components:t})):r.a.createElement(b,s({ref:n},d))}));function b(e,n){var t=arguments,a=n&&n.mdxType;if("string"==typeof e||a){var o=t.length,i=new Array(o);i[0]=h;var s={};for(var c in n)hasOwnProperty.call(n,c)&&(s[c]=n[c]);s.originalType=e,s.mdxType="string"==typeof e?e:a,i[1]=s;for(var d=2;d<o;d++)i[d]=t[d];return r.a.createElement.apply(null,i)}return r.a.createElement.apply(null,t)}h.displayName="MDXCreateElement"}}]);