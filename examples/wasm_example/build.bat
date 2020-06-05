cd ..\..\buildtools
emmake make -f Makefile.linux CXX="emcc -O2 -s WASM=1" AR="emcc -O2 -o" SUFFIX=.o FLAGS="-DSLEEPY_CUSTOM_CLIENT" ISCUSTOM="TRUE"
xcopy libsleepy_discord.o ..\examples\wasm_example /y
cd ..\examples\wasm_example
em++ -O2 -std=c++11 -I ../../include -I ../../include/sleepy_discord/IncludeNonexistent libsleepy_discord.o wasm_websocket.cpp --pre-js mod.js --js-library websocket.js --js-library session.js -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --bind -o wasm_discord.js