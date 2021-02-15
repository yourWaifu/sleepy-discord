(window.webpackJsonp=window.webpackJsonp||[]).push([[99],{172:function(e,t,n){"use strict";n.r(t),n.d(t,"frontMatter",(function(){return u})),n.d(t,"metadata",(function(){return a})),n.d(t,"toc",(function(){return c})),n.d(t,"default",(function(){return p}));var r=n(3),o=n(7),i=(n(0),n(321)),u={title:"SleepyDiscord::BaseAudioOutput"},a={unversionedId:"reference/Classes/struct_sleepy_discord_1_1_base_audio_output",id:"reference/Classes/struct_sleepy_discord_1_1_base_audio_output",isDocsHomePage:!1,title:"SleepyDiscord::BaseAudioOutput",description:"Public Types Documentation",source:"@site/docs\\reference\\Classes\\struct_sleepy_discord_1_1_base_audio_output.md",slug:"/reference/Classes/struct_sleepy_discord_1_1_base_audio_output",permalink:"/sleepy-discord-docs/docs/reference/Classes/struct_sleepy_discord_1_1_base_audio_output",version:"current",sidebar:"Reference",previous:{title:"SleepyDiscord::AuditLogKeyData",permalink:"/sleepy-discord-docs/docs/reference/Classes/struct_sleepy_discord_1_1_audit_log_key_data"},next:{title:"SleepyDiscord::BaseAudioSource",permalink:"/sleepy-discord-docs/docs/reference/Classes/struct_sleepy_discord_1_1_base_audio_source"}},c=[{value:"Public Types Documentation",id:"public-types-documentation",children:[{value:"using Container",id:"using-container",children:[]}]},{value:"Public Functions Documentation",id:"public-functions-documentation",children:[{value:"function BaseAudioOutput",id:"function-baseaudiooutput",children:[]},{value:"function ~BaseAudioOutput",id:"function-baseaudiooutput-1",children:[]},{value:"function write",id:"function-write",children:[]}]}],s={toc:c};function p(e){var t=e.components,n=Object(o.a)(e,["components"]);return Object(i.b)("wrapper",Object(r.a)({},s,n,{components:t,mdxType:"MDXLayout"}),Object(i.b)("h2",{id:"public-types-documentation"},"Public Types Documentation"),Object(i.b)("h3",{id:"using-container"},"using Container"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"using SleepyDiscord::BaseAudioOutput::Container =  std::array<AudioSample, AudioTransmissionDetails::proposedLength()>;\n")),Object(i.b)("h2",{id:"public-functions-documentation"},"Public Functions Documentation"),Object(i.b)("h3",{id:"function-baseaudiooutput"},"function BaseAudioOutput"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"BaseAudioOutput() =default\n")),Object(i.b)("h3",{id:"function-baseaudiooutput-1"},"function ~BaseAudioOutput"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"virtual ~BaseAudioOutput() =default\n")),Object(i.b)("h3",{id:"function-write"},"function write"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline virtual void write(\n    Container audio,\n    AudioTransmissionDetails & details\n)\n")),Object(i.b)("hr",null),Object(i.b)("p",null,"Updated on  9 February 2021 at 07:04:57 Eastern Standard Time"))}p.isMDXComponent=!0},321:function(e,t,n){"use strict";n.d(t,"a",(function(){return l})),n.d(t,"b",(function(){return f}));var r=n(0),o=n.n(r);function i(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function u(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var r=Object.getOwnPropertySymbols(e);t&&(r=r.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,r)}return n}function a(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?u(Object(n),!0).forEach((function(t){i(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):u(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function c(e,t){if(null==e)return{};var n,r,o=function(e,t){if(null==e)return{};var n,r,o={},i=Object.keys(e);for(r=0;r<i.length;r++)n=i[r],t.indexOf(n)>=0||(o[n]=e[n]);return o}(e,t);if(Object.getOwnPropertySymbols){var i=Object.getOwnPropertySymbols(e);for(r=0;r<i.length;r++)n=i[r],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(o[n]=e[n])}return o}var s=o.a.createContext({}),p=function(e){var t=o.a.useContext(s),n=t;return e&&(n="function"==typeof e?e(t):a(a({},t),e)),n},l=function(e){var t=p(e.components);return o.a.createElement(s.Provider,{value:t},e.children)},d={inlineCode:"code",wrapper:function(e){var t=e.children;return o.a.createElement(o.a.Fragment,{},t)}},b=o.a.forwardRef((function(e,t){var n=e.components,r=e.mdxType,i=e.originalType,u=e.parentName,s=c(e,["components","mdxType","originalType","parentName"]),l=p(n),b=r,f=l["".concat(u,".").concat(b)]||l[b]||d[b]||i;return n?o.a.createElement(f,a(a({ref:t},s),{},{components:n})):o.a.createElement(f,a({ref:t},s))}));function f(e,t){var n=arguments,r=t&&t.mdxType;if("string"==typeof e||r){var i=n.length,u=new Array(i);u[0]=b;var a={};for(var c in t)hasOwnProperty.call(t,c)&&(a[c]=t[c]);a.originalType=e,a.mdxType="string"==typeof e?e:r,u[1]=a;for(var s=2;s<i;s++)u[s]=n[s];return o.a.createElement.apply(null,u)}return o.a.createElement.apply(null,n)}b.displayName="MDXCreateElement"}}]);