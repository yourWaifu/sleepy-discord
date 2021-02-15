(window.webpackJsonp=window.webpackJsonp||[]).push([[61],{133:function(e,t,n){"use strict";n.r(t),n.d(t,"frontMatter",(function(){return s})),n.d(t,"metadata",(function(){return i})),n.d(t,"toc",(function(){return c})),n.d(t,"default",(function(){return p}));var a=n(3),r=n(7),o=(n(0),n(321)),s={title:"Snowflakes and IDs"},i={unversionedId:"snowflake",id:"snowflake",isDocsHomePage:!1,title:"Snowflakes and IDs",description:"Snowflakes very common type is used for identification. Defined as SleepyDiscord::Snowflake in snowflake.h.",source:"@site/docs\\snowflake.md",slug:"/snowflake",permalink:"/sleepy-discord-docs/docs/snowflake",version:"current",lastUpdatedAt:1613196460,sidebar:"Docs",previous:{title:"Unicode and Emojis",permalink:"/sleepy-discord-docs/docs/unicode"},next:{title:"Classes",permalink:"/sleepy-discord-docs/docs/reference/index_classes"}},c=[{value:"Unique",id:"unique",children:[]},{value:"Valid",id:"valid",children:[]},{value:"Casting",id:"casting",children:[{value:"To Snowflake",id:"to-snowflake",children:[]},{value:"To String or Int",id:"to-string-or-int",children:[]}]},{value:"Timestamp",id:"timestamp",children:[]},{value:"Equality ==",id:"equality-",children:[]}],l={toc:c};function p(e){var t=e.components,n=Object(r.a)(e,["components"]);return Object(o.b)("wrapper",Object(a.a)({},l,n,{components:t,mdxType:"MDXLayout"}),Object(o.b)("p",null,"Snowflakes very common type is used for identification. Defined as ",Object(o.b)("a",{parentName:"p",href:"reference/Classes/struct_sleepy_discord_1_1_snowflake"},"SleepyDiscord::Snowflake")," in ",Object(o.b)("a",{parentName:"p",href:"reference/Files/snowflake_8h#file-snowflake.h"},"snowflake.h"),"."),Object(o.b)("h2",{id:"unique"},"Unique"),Object(o.b)("p",null,"2 IDs for 2 different objects of the ",Object(o.b)("strong",{parentName:"p"},"same type")," should not have the same ID. However, 2 ID for objects of ",Object(o.b)("strong",{parentName:"p"},"different types")," can have the same ID. As such the library uses a template class to help differentiate the two. Often a Server and it's general channel and it's default role has the same ID, so it's useful for getting objects that would be created when another object is created on the same operation."),Object(o.b)("h2",{id:"valid"},"Valid"),Object(o.b)("p",null,"For empty IDs, the library uses an empty string to represent when an ID isn't available. For example, a Direct Message Channel would have an no Server ID and as such it's ID is an empty string."),Object(o.b)("p",null,"However, if it's not empty, the only way to know if it's valid is by giving the ID to Discord."),Object(o.b)("pre",null,Object(o.b)("code",{parentName:"pre",className:"language-cpp"},"if (messageID.empty())\n")),Object(o.b)("h2",{id:"casting"},"Casting"),Object(o.b)("h3",{id:"to-snowflake"},"To Snowflake"),Object(o.b)("pre",null,Object(o.b)("code",{parentName:"pre",className:"language-cpp"},'Snowflake<Message> ID = "string";\nSnowflake<Message> ID = message; //Same as message.ID\nSnowflake<Message> ID = 90485;\n')),Object(o.b)("h3",{id:"to-string-or-int"},"To String or Int"),Object(o.b)("p",null,"The Snowflake stores it's data in a string that you can cast to. You can also get a number if you like instead."),Object(o.b)("pre",null,Object(o.b)("code",{parentName:"pre",className:"language-cpp"},"std::string ID = messageID; //implicit\nstd::string ID = messagesID.string();\nint64_t ID = messageID.number();\n")),Object(o.b)("h2",{id:"timestamp"},"Timestamp"),Object(o.b)("p",null,"Snowflakes contains some data that tells you when an object was create, and the worker and process that created it. The time is what's useful tho, so the library can extract this data for you."),Object(o.b)("pre",null,Object(o.b)("code",{parentName:"pre",className:"language-cpp"},"std::chrono::time_point<std::chrono::steady_clock> birth = messageID.timestamp();\n")),Object(o.b)("h2",{id:"equality-"},"Equality =="),Object(o.b)("p",null,"This lets you check if you have the right object."),Object(o.b)("pre",null,Object(o.b)("code",{parentName:"pre",className:"language-cpp"},"if (message.channelID == channel.ID)\n")))}p.isMDXComponent=!0},321:function(e,t,n){"use strict";n.d(t,"a",(function(){return u})),n.d(t,"b",(function(){return b}));var a=n(0),r=n.n(a);function o(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function s(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var a=Object.getOwnPropertySymbols(e);t&&(a=a.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,a)}return n}function i(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?s(Object(n),!0).forEach((function(t){o(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):s(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function c(e,t){if(null==e)return{};var n,a,r=function(e,t){if(null==e)return{};var n,a,r={},o=Object.keys(e);for(a=0;a<o.length;a++)n=o[a],t.indexOf(n)>=0||(r[n]=e[n]);return r}(e,t);if(Object.getOwnPropertySymbols){var o=Object.getOwnPropertySymbols(e);for(a=0;a<o.length;a++)n=o[a],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(r[n]=e[n])}return r}var l=r.a.createContext({}),p=function(e){var t=r.a.useContext(l),n=t;return e&&(n="function"==typeof e?e(t):i(i({},t),e)),n},u=function(e){var t=p(e.components);return r.a.createElement(l.Provider,{value:t},e.children)},d={inlineCode:"code",wrapper:function(e){var t=e.children;return r.a.createElement(r.a.Fragment,{},t)}},f=r.a.forwardRef((function(e,t){var n=e.components,a=e.mdxType,o=e.originalType,s=e.parentName,l=c(e,["components","mdxType","originalType","parentName"]),u=p(n),f=a,b=u["".concat(s,".").concat(f)]||u[f]||d[f]||o;return n?r.a.createElement(b,i(i({ref:t},l),{},{components:n})):r.a.createElement(b,i({ref:t},l))}));function b(e,t){var n=arguments,a=t&&t.mdxType;if("string"==typeof e||a){var o=n.length,s=new Array(o);s[0]=f;var i={};for(var c in t)hasOwnProperty.call(t,c)&&(i[c]=t[c]);i.originalType=e,i.mdxType="string"==typeof e?e:a,s[1]=i;for(var l=2;l<o;l++)s[l]=n[l];return r.a.createElement.apply(null,s)}return r.a.createElement.apply(null,n)}f.displayName="MDXCreateElement"}}]);