(window.webpackJsonp=window.webpackJsonp||[]).push([[286],{356:function(e,n,t){"use strict";t.r(n),t.d(n,"frontMatter",(function(){return a})),t.d(n,"metadata",(function(){return o})),t.d(n,"toc",(function(){return l})),t.d(n,"default",(function(){return p}));var c=t(3),r=t(7),i=(t(0),t(364)),a={title:"SleepyDiscord::Cache"},o={unversionedId:"reference/Classes/struct_sleepy_discord_1_1_cache",id:"reference/Classes/struct_sleepy_discord_1_1_cache",isDocsHomePage:!1,title:"SleepyDiscord::Cache",description:"More...",source:"@site/docs/reference/Classes/struct_sleepy_discord_1_1_cache.md",slug:"/reference/Classes/struct_sleepy_discord_1_1_cache",permalink:"/sleepy-discord/docs/reference/Classes/struct_sleepy_discord_1_1_cache",version:"current",sidebar:"Reference",previous:{title:"SleepyDiscord::BooleanResponse",permalink:"/sleepy-discord/docs/reference/Classes/struct_sleepy_discord_1_1_boolean_response"},next:{title:"SleepyDiscord::Channel",permalink:"/sleepy-discord/docs/reference/Classes/struct_sleepy_discord_1_1_channel"}},l=[{value:"Detailed Description",id:"detailed-description",children:[]},{value:"Public Types Documentation",id:"public-types-documentation",children:[{value:"using Type",id:"using-type",children:[]},{value:"using Parent",id:"using-parent",children:[]},{value:"using Key",id:"using-key",children:[]}]},{value:"Public Functions Documentation",id:"public-functions-documentation",children:[{value:"function Cache",id:"function-cache",children:[]},{value:"function Cache",id:"function-cache-1",children:[]},{value:"function Cache",id:"function-cache-2",children:[]},{value:"function begin",id:"function-begin",children:[]},{value:"function end",id:"function-end",children:[]},{value:"function begin",id:"function-begin-1",children:[]},{value:"function end",id:"function-end-1",children:[]},{value:"function findOneWithObject",id:"function-findonewithobject",children:[]},{value:"function insert",id:"function-insert",children:[]},{value:"function emplace",id:"function-emplace",children:[]},{value:"function find",id:"function-find",children:[]},{value:"function find",id:"function-find-1",children:[]},{value:"function erase",id:"function-erase",children:[]},{value:"function erase",id:"function-erase-1",children:[]}]}],u={toc:l};function p(e){var n=e.components,t=Object(r.a)(e,["components"]);return Object(i.b)("wrapper",Object(c.a)({},u,t,{components:n,mdxType:"MDXLayout"}),Object(i.b)("p",null," ",Object(i.b)("a",{parentName:"p",href:"#detailed-description"},"More...")),Object(i.b)("p",null,"Inherits from CacheParent< _Type >"),Object(i.b)("h2",{id:"detailed-description"},"Detailed Description"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"template <class _Type >\nstruct SleepyDiscord::Cache;\n")),Object(i.b)("h2",{id:"public-types-documentation"},"Public Types Documentation"),Object(i.b)("h3",{id:"using-type"},"using Type"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"using SleepyDiscord::Cache< _Type >::Type =  _Type;\n")),Object(i.b)("h3",{id:"using-parent"},"using Parent"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"using SleepyDiscord::Cache< _Type >::Parent =  CacheParent<_Type>;\n")),Object(i.b)("h3",{id:"using-key"},"using Key"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"using SleepyDiscord::Cache< _Type >::Key =  typename Snowflake<Type>::RawType;\n")),Object(i.b)("h2",{id:"public-functions-documentation"},"Public Functions Documentation"),Object(i.b)("h3",{id:"function-cache"},"function Cache"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline Cache()\n")),Object(i.b)("h3",{id:"function-cache-1"},"function Cache"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline Cache(\n    Parent map\n)\n")),Object(i.b)("h3",{id:"function-cache-2"},"function Cache"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"template <class InputIterator >\ninline Cache(\n    InputIterator first,\n    InputIterator last\n)\n")),Object(i.b)("h3",{id:"function-begin"},"function begin"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline iterator begin()\n")),Object(i.b)("h3",{id:"function-end"},"function end"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline iterator end()\n")),Object(i.b)("h3",{id:"function-begin-1"},"function begin"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline const_iterator begin() const\n")),Object(i.b)("h3",{id:"function-end-1"},"function end"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline const_iterator end() const\n")),Object(i.b)("h3",{id:"function-findonewithobject"},"function findOneWithObject"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"template <class Container ,\nclass Object >\ninline const_iterator findOneWithObject(\n    Container Type::* list,\n    const Snowflake< Object > & objectID\n)\n")),Object(i.b)("h3",{id:"function-insert"},"function insert"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline std::pair< iterator, bool > insert(\n    Type & value\n)\n")),Object(i.b)("h3",{id:"function-emplace"},"function emplace"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"template <class... Args>\ninline std::pair< iterator, bool > emplace(\n    const json::Value & value\n)\n")),Object(i.b)("h3",{id:"function-find"},"function find"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline iterator find(\n    const Key & key\n)\n")),Object(i.b)("h3",{id:"function-find-1"},"function find"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline const_iterator find(\n    const Key & key\n) const\n")),Object(i.b)("h3",{id:"function-erase"},"function erase"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline iterator erase(\n    const_iterator pos\n)\n")),Object(i.b)("h3",{id:"function-erase-1"},"function erase"),Object(i.b)("pre",null,Object(i.b)("code",{parentName:"pre",className:"language-cpp"},"inline iterator erase(\n    const_iterator first,\n    const_iterator last\n)\n")),Object(i.b)("hr",null),Object(i.b)("p",null,"Updated on 30 April 2021 at 23:22:25 UTC"))}p.isMDXComponent=!0},364:function(e,n,t){"use strict";t.d(n,"a",(function(){return s})),t.d(n,"b",(function(){return f}));var c=t(0),r=t.n(c);function i(e,n,t){return n in e?Object.defineProperty(e,n,{value:t,enumerable:!0,configurable:!0,writable:!0}):e[n]=t,e}function a(e,n){var t=Object.keys(e);if(Object.getOwnPropertySymbols){var c=Object.getOwnPropertySymbols(e);n&&(c=c.filter((function(n){return Object.getOwnPropertyDescriptor(e,n).enumerable}))),t.push.apply(t,c)}return t}function o(e){for(var n=1;n<arguments.length;n++){var t=null!=arguments[n]?arguments[n]:{};n%2?a(Object(t),!0).forEach((function(n){i(e,n,t[n])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(t)):a(Object(t)).forEach((function(n){Object.defineProperty(e,n,Object.getOwnPropertyDescriptor(t,n))}))}return e}function l(e,n){if(null==e)return{};var t,c,r=function(e,n){if(null==e)return{};var t,c,r={},i=Object.keys(e);for(c=0;c<i.length;c++)t=i[c],n.indexOf(t)>=0||(r[t]=e[t]);return r}(e,n);if(Object.getOwnPropertySymbols){var i=Object.getOwnPropertySymbols(e);for(c=0;c<i.length;c++)t=i[c],n.indexOf(t)>=0||Object.prototype.propertyIsEnumerable.call(e,t)&&(r[t]=e[t])}return r}var u=r.a.createContext({}),p=function(e){var n=r.a.useContext(u),t=n;return e&&(t="function"==typeof e?e(n):o(o({},n),e)),t},s=function(e){var n=p(e.components);return r.a.createElement(u.Provider,{value:n},e.children)},d={inlineCode:"code",wrapper:function(e){var n=e.children;return r.a.createElement(r.a.Fragment,{},n)}},b=r.a.forwardRef((function(e,n){var t=e.components,c=e.mdxType,i=e.originalType,a=e.parentName,u=l(e,["components","mdxType","originalType","parentName"]),s=p(t),b=c,f=s["".concat(a,".").concat(b)]||s[b]||d[b]||i;return t?r.a.createElement(f,o(o({ref:n},u),{},{components:t})):r.a.createElement(f,o({ref:n},u))}));function f(e,n){var t=arguments,c=n&&n.mdxType;if("string"==typeof e||c){var i=t.length,a=new Array(i);a[0]=b;var o={};for(var l in n)hasOwnProperty.call(n,l)&&(o[l]=n[l]);o.originalType=e,o.mdxType="string"==typeof e?e:c,a[1]=o;for(var u=2;u<i;u++)a[u]=t[u];return r.a.createElement.apply(null,a)}return r.a.createElement.apply(null,t)}b.displayName="MDXCreateElement"}}]);