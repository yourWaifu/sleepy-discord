"use strict";(self.webpackChunksleepy_discord_docs=self.webpackChunksleepy_discord_docs||[]).push([[3623],{3905:function(e,t,n){n.d(t,{Zo:function(){return u},kt:function(){return f}});var r=n(7294);function a(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function i(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var r=Object.getOwnPropertySymbols(e);t&&(r=r.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,r)}return n}function c(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?i(Object(n),!0).forEach((function(t){a(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):i(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function l(e,t){if(null==e)return{};var n,r,a=function(e,t){if(null==e)return{};var n,r,a={},i=Object.keys(e);for(r=0;r<i.length;r++)n=i[r],t.indexOf(n)>=0||(a[n]=e[n]);return a}(e,t);if(Object.getOwnPropertySymbols){var i=Object.getOwnPropertySymbols(e);for(r=0;r<i.length;r++)n=i[r],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(a[n]=e[n])}return a}var o=r.createContext({}),s=function(e){var t=r.useContext(o),n=t;return e&&(n="function"==typeof e?e(t):c(c({},t),e)),n},u=function(e){var t=s(e.components);return r.createElement(o.Provider,{value:t},e.children)},p={inlineCode:"code",wrapper:function(e){var t=e.children;return r.createElement(r.Fragment,{},t)}},d=r.forwardRef((function(e,t){var n=e.components,a=e.mdxType,i=e.originalType,o=e.parentName,u=l(e,["components","mdxType","originalType","parentName"]),d=s(n),f=a,y=d["".concat(o,".").concat(f)]||d[f]||p[f]||i;return n?r.createElement(y,c(c({ref:t},u),{},{components:n})):r.createElement(y,c({ref:t},u))}));function f(e,t){var n=arguments,a=t&&t.mdxType;if("string"==typeof e||a){var i=n.length,c=new Array(i);c[0]=d;var l={};for(var o in t)hasOwnProperty.call(t,o)&&(l[o]=t[o]);l.originalType=e,l.mdxType="string"==typeof e?e:a,c[1]=l;for(var s=2;s<i;s++)c[s]=n[s];return r.createElement.apply(null,c)}return r.createElement.apply(null,n)}d.displayName="MDXCreateElement"},4542:function(e,t,n){n.r(t),n.d(t,{frontMatter:function(){return l},contentTitle:function(){return o},metadata:function(){return s},toc:function(){return u},default:function(){return d}});var r=n(3117),a=n(102),i=(n(7294),n(3905)),c=["components"],l={title:"SleepyDiscord::Gateway"},o=void 0,s={unversionedId:"reference/Classes/struct_sleepy_discord_1_1_gateway",id:"reference/Classes/struct_sleepy_discord_1_1_gateway",title:"SleepyDiscord::Gateway",description:"Public Functions Documentation",source:"@site/docs/reference/Classes/struct_sleepy_discord_1_1_gateway.md",sourceDirName:"reference/Classes",slug:"/reference/Classes/struct_sleepy_discord_1_1_gateway",permalink:"/sleepy-discord/docs/reference/Classes/struct_sleepy_discord_1_1_gateway",tags:[],version:"current",frontMatter:{title:"SleepyDiscord::Gateway"},sidebar:"Reference",previous:{title:"SleepyDiscord::Game",permalink:"/sleepy-discord/docs/reference/Classes/struct_sleepy_discord_1_1_game"},next:{title:"SleepyDiscord::GenericWebsocketConnection",permalink:"/sleepy-discord/docs/reference/Classes/struct_sleepy_discord_1_1_generic_websocket_connection"}},u=[{value:"Public Functions Documentation",id:"public-functions-documentation",children:[{value:"function Gateway",id:"function-gateway",children:[],level:3},{value:"function Gateway",id:"function-gateway-1",children:[],level:3},{value:"function Gateway",id:"function-gateway-2",children:[],level:3},{value:"function std::make_tuple",id:"function-stdmake_tuple",children:[],level:3}],level:2},{value:"Public Attributes Documentation",id:"public-attributes-documentation",children:[{value:"variable url",id:"variable-url",children:[],level:3},{value:"variable shards",id:"variable-shards",children:[],level:3},{value:"variable sessionStartLimit",id:"variable-sessionstartlimit",children:[],level:3}],level:2}],p={toc:u};function d(e){var t=e.components,n=(0,a.Z)(e,c);return(0,i.kt)("wrapper",(0,r.Z)({},p,n,{components:t,mdxType:"MDXLayout"}),(0,i.kt)("h2",{id:"public-functions-documentation"},"Public Functions Documentation"),(0,i.kt)("h3",{id:"function-gateway"},"function Gateway"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-cpp"},"Gateway() =default\n")),(0,i.kt)("h3",{id:"function-gateway-1"},"function Gateway"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-cpp"},"Gateway(\n    const json::Value & rawJSON\n)\n")),(0,i.kt)("h3",{id:"function-gateway-2"},"function Gateway"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-cpp"},"inline Gateway(\n    const nonstd::string_view & json\n)\n")),(0,i.kt)("h3",{id:"function-stdmake_tuple"},"function std::make_tuple"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-cpp"},'JSONStructStart std::make_tuple(\n    json::pair &::, "", ::OPTIONAL_FIELD ,\n    json::pair &::, "", ::OPTIONAL_FIELD ,\n    json::pair &::, "", ::OPTIONAL_FIELD \n)\n')),(0,i.kt)("h2",{id:"public-attributes-documentation"},"Public Attributes Documentation"),(0,i.kt)("h3",{id:"variable-url"},"variable url"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-cpp"},"std::string url;\n")),(0,i.kt)("h3",{id:"variable-shards"},"variable shards"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-cpp"},"int shards;\n")),(0,i.kt)("h3",{id:"variable-sessionstartlimit"},"variable sessionStartLimit"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-cpp"},"SessionStartLimit sessionStartLimit;\n")),(0,i.kt)("hr",null),(0,i.kt)("p",null,"Updated on 21 January 2022 at 02:36:00 UTC"))}d.isMDXComponent=!0}}]);