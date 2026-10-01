#!/bin/sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$script_dir"

if command -v clang >/dev/null 2>&1; then
  compiler=$(command -v clang)
elif command -v xcrun >/dev/null 2>&1 && xcrun --find clang >/dev/null 2>&1; then
  compiler=$(xcrun --find clang)
else
  echo "Error: clang is required. Install the Xcode Command Line Tools with: xcode-select --install" >&2
  exit 1
fi

if command -v brew >/dev/null 2>&1; then
  brew_command=$(command -v brew)
elif [ -x /opt/homebrew/bin/brew ]; then
  brew_command=/opt/homebrew/bin/brew
elif [ -x /usr/local/bin/brew ]; then
  brew_command=/usr/local/bin/brew
else
  echo "Error: Homebrew is required. Install it from https://brew.sh, then run: brew install raylib" >&2
  exit 1
fi

if raylib_prefix=$("$brew_command" --prefix raylib 2>/dev/null); then
  :
elif [ -d /opt/homebrew/opt/raylib/include ] && [ -d /opt/homebrew/opt/raylib/lib ]; then
  raylib_prefix=/opt/homebrew/opt/raylib
elif [ -d /usr/local/opt/raylib/include ] && [ -d /usr/local/opt/raylib/lib ]; then
  raylib_prefix=/usr/local/opt/raylib
else
  echo "Error: raylib is not installed. Run: $brew_command install raylib" >&2
  exit 1
fi

mkdir -p build
"$compiler" -Wall -Wextra -Wpedantic -std=c99 -O2 \
  src/core/*.c src/audio/*.c src/screens/*.c src/render/*.c \
  -o build/sokoban \
  -Isrc/core -Isrc/audio -Isrc/screens -Isrc/render \
  -I"$raylib_prefix/include" \
  -L"$raylib_prefix/lib" \
  -lraylib \
  -framework CoreVideo \
  -framework IOKit \
  -framework Cocoa \
  -framework OpenGL

if [ "${1-}" = "--build-only" ]; then
  echo "Build complete: $script_dir/build/sokoban"
  exit 0
fi

exec "$script_dir/build/sokoban"
