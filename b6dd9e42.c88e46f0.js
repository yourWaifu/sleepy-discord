(window.webpackJsonp=window.webpackJsonp||[]).push([[195],{266:function(e,n,t){"use strict";t.r(n),t.d(n,"frontMatter",(function(){return i})),t.d(n,"metadata",(function(){return o})),t.d(n,"toc",(function(){return l})),t.d(n,"default",(function(){return d}));var r=t(3),c=t(7),a=(t(0),t(364)),i={title:"include/sleepy_discord/udp.h"},o={unversionedId:"reference/Files/udp_8h",id:"reference/Files/udp_8h",isDocsHomePage:!1,title:"include/sleepy_discord/udp.h",description:"Namespaces",source:"@site/docs/reference/Files/udp_8h.md",slug:"/reference/Files/udp_8h",permalink:"/sleepy-discord/docs/reference/Files/udp_8h",version:"current",sidebar:"Reference",previous:{title:"include/sleepy_discord/timer.h",permalink:"/sleepy-discord/docs/reference/Files/timer_8h"},next:{title:"include/sleepy_discord/udp_client.h",permalink:"/sleepy-discord/docs/reference/Files/udp__client_8h"}},l=[{value:"Namespaces",id:"namespaces",children:[]},{value:"Classes",id:"classes",children:[]},{value:"Source code",id:"source-code",children:[]}],s={toc:l};function d(e){var n=e.components,t=Object(c.a)(e,["components"]);return Object(a.b)("wrapper",Object(r.a)({},s,t,{components:n,mdxType:"MDXLayout"}),Object(a.b)("h2",{id:"namespaces"},"Namespaces"),Object(a.b)("table",null,Object(a.b)("thead",{parentName:"table"},Object(a.b)("tr",{parentName:"thead"},Object(a.b)("th",{parentName:"tr",align:null},"Name"))),Object(a.b)("tbody",{parentName:"table"},Object(a.b)("tr",{parentName:"tbody"},Object(a.b)("td",{parentName:"tr",align:null},Object(a.b)("strong",{parentName:"td"},Object(a.b)("a",{parentName:"strong",href:"/docs/reference/Namespaces/namespace_sleepy_discord"},"SleepyDiscord")))))),Object(a.b)("h2",{id:"classes"},"Classes"),Object(a.b)("table",null,Object(a.b)("thead",{parentName:"table"},Object(a.b)("tr",{parentName:"thead"},Object(a.b)("th",{parentName:"tr",align:null}),Object(a.b)("th",{parentName:"tr",align:null},"Name"))),Object(a.b)("tbody",{parentName:"table"},Object(a.b)("tr",{parentName:"tbody"},Object(a.b)("td",{parentName:"tr",align:null},"class"),Object(a.b)("td",{parentName:"tr",align:null},Object(a.b)("strong",{parentName:"td"},Object(a.b)("a",{parentName:"strong",href:"/docs/reference/Classes/class_sleepy_discord_1_1_generic_u_d_p_client"},"SleepyDiscord::GenericUDPClient")))))),Object(a.b)("h2",{id:"source-code"},"Source code"),Object(a.b)("pre",null,Object(a.b)("code",{parentName:"pre",className:"language-cpp"},"#pragma once\n#include <string>\n#include <cstdint>\n#include <vector>\n#include <functional>\n\nnamespace SleepyDiscord {\n    class GenericUDPClient {\n    public:\n        typedef std::function<void()> SendHandler;\n        typedef std::function<void(const std::vector<uint8_t>&)> ReceiveHandler;\n\n        virtual bool connect(const std::string& to, const uint16_t port) = 0;\n        virtual void send(\n            const uint8_t* buffer,\n            size_t bufferLength,\n            SendHandler handler = [](){}\n        ) = 0;\n        virtual void receive(ReceiveHandler handler) = 0;\n\n        inline void send(const std::vector<uint8_t> buffer, SendHandler handler = [](){}) {\n            send(&buffer[0], buffer.size(), handler);\n        }\n    };\n}\n")),Object(a.b)("hr",null),Object(a.b)("p",null,"Updated on 10 April 2021 at 22:46:48 UTC"))}d.isMDXComponent=!0},364:function(e,n,t){"use strict";t.d(n,"a",(function(){return p})),t.d(n,"b",(function(){return f}));var r=t(0),c=t.n(r);function a(e,n,t){return n in e?Object.defineProperty(e,n,{value:t,enumerable:!0,configurable:!0,writable:!0}):e[n]=t,e}function i(e,n){var t=Object.keys(e);if(Object.getOwnPropertySymbols){var r=Object.getOwnPropertySymbols(e);n&&(r=r.filter((function(n){return Object.getOwnPropertyDescriptor(e,n).enumerable}))),t.push.apply(t,r)}return t}function o(e){for(var n=1;n<arguments.length;n++){var t=null!=arguments[n]?arguments[n]:{};n%2?i(Object(t),!0).forEach((function(n){a(e,n,t[n])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(t)):i(Object(t)).forEach((function(n){Object.defineProperty(e,n,Object.getOwnPropertyDescriptor(t,n))}))}return e}function l(e,n){if(null==e)return{};var t,r,c=function(e,n){if(null==e)return{};var t,r,c={},a=Object.keys(e);for(r=0;r<a.length;r++)t=a[r],n.indexOf(t)>=0||(c[t]=e[t]);return c}(e,n);if(Object.getOwnPropertySymbols){var a=Object.getOwnPropertySymbols(e);for(r=0;r<a.length;r++)t=a[r],n.indexOf(t)>=0||Object.prototype.propertyIsEnumerable.call(e,t)&&(c[t]=e[t])}return c}var s=c.a.createContext({}),d=function(e){var n=c.a.useContext(s),t=n;return e&&(t="function"==typeof e?e(n):o(o({},n),e)),t},p=function(e){var n=d(e.components);return c.a.createElement(s.Provider,{value:n},e.children)},u={inlineCode:"code",wrapper:function(e){var n=e.children;return c.a.createElement(c.a.Fragment,{},n)}},b=c.a.forwardRef((function(e,n){var t=e.components,r=e.mdxType,a=e.originalType,i=e.parentName,s=l(e,["components","mdxType","originalType","parentName"]),p=d(t),b=r,f=p["".concat(i,".").concat(b)]||p[b]||u[b]||a;return t?c.a.createElement(f,o(o({ref:n},s),{},{components:t})):c.a.createElement(f,o({ref:n},s))}));function f(e,n){var t=arguments,r=n&&n.mdxType;if("string"==typeof e||r){var a=t.length,i=new Array(a);i[0]=b;var o={};for(var l in n)hasOwnProperty.call(n,l)&&(o[l]=n[l]);o.originalType=e,o.mdxType="string"==typeof e?e:r,i[1]=o;for(var s=2;s<a;s++)i[s]=t[s];return c.a.createElement.apply(null,i)}return c.a.createElement.apply(null,t)}b.displayName="MDXCreateElement"}}]);