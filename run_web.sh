#!/bin/sh
# Builds the browser (WebAssembly) version of the game into build/web/.
#
# Needs the Emscripten SDK and a raylib built for PLATFORM_WEB. Point
# EMSDK at the SDK and RAYLIB_WEB at the directory holding
# libraylib.web.a and raylib.h, or let the defaults below find them.
#
# raylib must be built with GRAPHICS=GRAPHICS_API_OPENGL_ES3, not ES2: on an
# ES2/WebGL1 context raylib finds no vertex-array-object support and falls back
# to a per-draw path that binds a vertex attribute the default shader does not
# have. That logs two WebGL warnings for every batch, thousands per second, and
# drags the frame rate to single digits.
#
#   ./run_web.sh              build, then serve on http://localhost:8080
#   ./run_web.sh --build-only build only
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$script_dir"

emsdk_dir=${EMSDK:-$HOME/emsdk}
if [ ! -f "$emsdk_dir/emsdk_env.sh" ]; then
  echo "Error: Emscripten SDK not found at $emsdk_dir." >&2
  echo "Install it, or set EMSDK to its location:" >&2
  echo "  git clone https://github.com/emscripten-core/emsdk.git ~/emsdk" >&2
  echo "  cd ~/emsdk && ./emsdk install latest && ./emsdk activate latest" >&2
  exit 1
fi

# emsdk.py detects macOS through platform.mac_ver(), which returns empty under
# some Homebrew Python builds. The override costs nothing when detection works.
case $(uname -s) in
  Darwin) EMSDK_OS=${EMSDK_OS:-macos}; export EMSDK_OS ;;
esac

# shellcheck disable=SC1091
. "$emsdk_dir/emsdk_env.sh" >/dev/null 2>&1

if ! command -v emcc >/dev/null 2>&1; then
  echo "Error: emcc is not on PATH after sourcing $emsdk_dir/emsdk_env.sh." >&2
  exit 1
fi

raylib_web=${RAYLIB_WEB:-}
if [ -z "$raylib_web" ]; then
  for candidate in \
    "$script_dir/lib/web" \
    "$script_dir/../raylib/src" \
    "$HOME/raylib/src"
  do
    if [ -f "$candidate/libraylib.web.a" ]; then
      raylib_web=$candidate
      break
    fi
  done
fi

if [ -z "$raylib_web" ] || [ ! -f "$raylib_web/libraylib.web.a" ]; then
  echo "Error: libraylib.web.a not found. Set RAYLIB_WEB to the directory" >&2
  echo "holding it, or build raylib for the web first:" >&2
  echo "  curl -L -o raylib.tar.gz https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz" >&2
  echo "  tar xzf raylib.tar.gz && cd raylib-6.0/src" >&2
  echo "  make PLATFORM=PLATFORM_WEB GRAPHICS=GRAPHICS_API_OPENGL_ES3" >&2
  exit 1
fi

raylib_include=$raylib_web
if [ ! -f "$raylib_include/raylib.h" ] &&
   [ -f "$raylib_web/../include/raylib.h" ]; then
  raylib_include=$raylib_web/../include
fi
if [ ! -f "$raylib_include/raylib.h" ]; then
  echo "Error: raylib.h not found beside libraylib.web.a or in ../include." >&2
  exit 1
fi

mkdir -p build/web

# The browser receives resized art. Only the menu is preloaded; the rest is
# fetched in the background after the game starts.
./tools/stage_web_assets.sh build/web/assets
python3 tools/split_web_assets.py build/web/assets build/web/startup-assets build/web

echo "Compiling to WebAssembly..."
emcc -o build/web/sokoban.js \
  src/core/*.c src/audio/*.c src/screens/*.c src/render/*.c \
  -Isrc/core -Isrc/audio -Isrc/screens -Isrc/render \
  -I"$raylib_include" \
  "$raylib_web/libraylib.web.a" \
  -DPLATFORM_WEB \
  -std=c99 -O2 \
  -Wall -Wextra \
  -sUSE_GLFW=3 \
  -sMIN_WEBGL_VERSION=2 \
  -sMAX_WEBGL_VERSION=2 \
  -sALLOW_MEMORY_GROWTH=1 \
  -sINITIAL_MEMORY=268435456 \
  -sSTACK_SIZE=1048576 \
  -sFORCE_FILESYSTEM=1 \
  -sMODULARIZE=1 \
  -sEXPORT_ES6=0 \
  -sEXPORT_NAME=createSokoban \
  -sEXPORTED_RUNTIME_METHODS=FS,IDBFS,addRunDependency,removeRunDependency \
  -lidbfs.js \
  --pre-js tools/web/pre.js \
  --preload-file build/web/startup-assets@/assets

cp tools/web/shell.html build/web/index.html
cp tools/web/vercel.json build/web/vercel.json
rm -rf build/web/assets build/web/startup-assets

( cd build/web && zip -q -X -r ../../sokoban-web.zip \
    index.html sokoban.js sokoban.wasm sokoban.data content.json content.data )

echo "Build complete: $script_dir/build/web/index.html"
du -ch build/web/sokoban.js build/web/sokoban.wasm build/web/sokoban.data \
  2>/dev/null | tail -1 | sed 's/^/Payload: /'

if [ "${1-}" = "--build-only" ]; then
  exit 0
fi

# The .wasm and .data files need real HTTP; opening index.html as a file:// URL
# will not work.
echo "Serving http://localhost:8080/ (Ctrl-C to stop)"
cd build/web && exec python3 -m http.server 8080
