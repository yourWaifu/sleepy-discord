(window.webpackJsonp=window.webpackJsonp||[]).push([[73],{145:function(e,t,n){"use strict";n.r(t),n.d(t,"frontMatter",(function(){return i})),n.d(t,"metadata",(function(){return l})),n.d(t,"toc",(function(){return c})),n.d(t,"default",(function(){return d}));var o=n(3),r=n(7),a=(n(0),n(321)),i={title:"Common Errors"},l={unversionedId:"common-build-errors",id:"common-build-errors",isDocsHomePage:!1,title:"Common Errors",description:"CA path only supported by ...",source:"@site/docs\\common-build-errors.md",slug:"/common-build-errors",permalink:"/sleepy-discord-docs/docs/common-build-errors",version:"current",lastUpdatedAt:1612689535,sidebar:"Docs",previous:{title:"Build",permalink:"/sleepy-discord-docs/docs/generator-compile"},next:{title:"Using only Sleepy Discord",permalink:"/sleepy-discord-docs/docs/only-sleepy-discord"}},c=[{value:"CA path only supported by ...",id:"ca-path-only-supported-by-",children:[]},{value:"No CMAKE_CXX_COMPILER ...",id:"no-cmake_cxx_compiler-",children:[]},{value:"sleepy-discord/CMakeLists.txt (add_subdirectory)",id:"sleepy-discordcmakeliststxt-add_subdirectory",children:[]},{value:"unofficial-sodium",id:"unofficial-sodium",children:[]}],s={toc:c};function d(e){var t=e.components,n=Object(r.a)(e,["components"]);return Object(a.b)("wrapper",Object(o.a)({},s,n,{components:t,mdxType:"MDXLayout"}),Object(a.b)("h3",{id:"ca-path-only-supported-by-"},"CA path only supported by ..."),Object(a.b)("pre",null,Object(a.b)("code",{parentName:"pre",className:"language-console"},"CA path only supported by OpenSSL, GnuTLS or mbed TLS.  Set CURL_CA_PATH=none or enable one of those TLS backends.\n")),Object(a.b)("p",null,"libCurl couldn't set the CURL_CA_PATH variable as it's not supported for the TLS backend that will be used."),Object(a.b)("p",null,"The easiest way to fix this is to install libCurl from a package manager. If that doesn't fix it, you'll need to enable a TLS backend for libCurl."),Object(a.b)("h3",{id:"no-cmake_cxx_compiler-"},"No CMAKE_CXX_COMPILER ..."),Object(a.b)("pre",null,Object(a.b)("code",{parentName:"pre",className:"language-console"},'No CMAKE_CXX_COMPILER could be found.\n\nTell CMake where to find the compiler by setting either the environment variable "CXX" or the CMake cache entry CMAKE_CXX_COMPILER to the full path to the compiler, or to the compiler name if it is in the PATH.\n')),Object(a.b)("p",null,"Usually this means you don't have a compiler installed. GCC, Clang, or MSVC are recommend. You can check if you have them installed by run the compiler's command in the terminal. If you have a compiler installed, set the environment variable ",Object(a.b)("inlineCode",{parentName:"p"},"CXX")," to the command for the C++ compiler or the path to the C++ compiler."),Object(a.b)("h3",{id:"sleepy-discordcmakeliststxt-add_subdirectory"},"sleepy-discord/CMakeLists.txt (add_subdirectory)"),Object(a.b)("pre",null,Object(a.b)("code",{parentName:"pre",className:"language-console"},"CMake Error at dependencies/sleepy-discord/CMakeLists.txt:176 (add_subdirectory):\n  The source directory\n\n    /path/to/sleepy-discord/deps/lib\n\n  does not contain a CMakeLists.txt file.\n")),Object(a.b)("p",null,"Usually this means, that something went wrong with the download for one of the library. The easiest solution is to delete the CMakeCache.txt in your build folder and running CMake again. If you want a different solution that would be to have CMake only download and configure that one lib, you can do this by open the CMakeCache.txt and removing the ",Object(a.b)("inlineCode",{parentName:"p"},"lib-name_BINARY_DIR:STATIC")," and ",Object(a.b)("inlineCode",{parentName:"p"},"lib-name_SOURCE_DIR:STATIC")," from the file. After that, save the file. Now run Cmake with ",Object(a.b)("inlineCode",{parentName:"p"},"AUTO_DOWNLOAD_LIBRARY:BOOL")," set to True, you can set this to False afterwards."),Object(a.b)("h3",{id:"unofficial-sodium"},"unofficial-sodium"),Object(a.b)("pre",null,Object(a.b)("code",{parentName:"pre",className:"language-console"},'  Could not find a package configuration file provided by "unofficial-sodium" with any of the following names:\n\n    unofficial-sodiumConfig.cmake\n    unofficial-sodium-config.cmake\n')),Object(a.b)("p",null,"This error only happens on windows as on Unix, make is used to compile libSodium but on windows things are a bit different. However, you can install libSodium using the unofficial sodium package from VCPKG."))}d.isMDXComponent=!0},321:function(e,t,n){"use strict";n.d(t,"a",(function(){return u})),n.d(t,"b",(function(){return m}));var o=n(0),r=n.n(o);function a(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function i(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var o=Object.getOwnPropertySymbols(e);t&&(o=o.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,o)}return n}function l(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?i(Object(n),!0).forEach((function(t){a(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):i(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function c(e,t){if(null==e)return{};var n,o,r=function(e,t){if(null==e)return{};var n,o,r={},a=Object.keys(e);for(o=0;o<a.length;o++)n=a[o],t.indexOf(n)>=0||(r[n]=e[n]);return r}(e,t);if(Object.getOwnPropertySymbols){var a=Object.getOwnPropertySymbols(e);for(o=0;o<a.length;o++)n=a[o],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(r[n]=e[n])}return r}var s=r.a.createContext({}),d=function(e){var t=r.a.useContext(s),n=t;return e&&(n="function"==typeof e?e(t):l(l({},t),e)),n},u=function(e){var t=d(e.components);return r.a.createElement(s.Provider,{value:t},e.children)},p={inlineCode:"code",wrapper:function(e){var t=e.children;return r.a.createElement(r.a.Fragment,{},t)}},b=r.a.forwardRef((function(e,t){var n=e.components,o=e.mdxType,a=e.originalType,i=e.parentName,s=c(e,["components","mdxType","originalType","parentName"]),u=d(n),b=o,m=u["".concat(i,".").concat(b)]||u[b]||p[b]||a;return n?r.a.createElement(m,l(l({ref:t},s),{},{components:n})):r.a.createElement(m,l({ref:t},s))}));function m(e,t){var n=arguments,o=t&&t.mdxType;if("string"==typeof e||o){var a=n.length,i=new Array(a);i[0]=b;var l={};for(var c in t)hasOwnProperty.call(t,c)&&(l[c]=t[c]);l.originalType=e,l.mdxType="string"==typeof e?e:o,i[1]=l;for(var s=2;s<a;s++)i[s]=n[s];return r.a.createElement.apply(null,i)}return r.a.createElement.apply(null,n)}b.displayName="MDXCreateElement"}}]);