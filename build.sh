#!/bin/bash
SCRIPT_PATH="$(cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
BUILD_PATH="$SCRIPT_PATH/cmake-build"

if [ "$1" == "-c" ]; then
  echo "== Cleaning full $BUILD_PATH =="
  rm -rf $BUILD_PATH
  shift
fi

mkdir -p $BUILD_PATH && cd $BUILD_PATH

if [ "$1" == "-d" ]; then
  shift
  LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH cmake -DCMAKE_BUILD_TYPE=Debug $SCRIPT_PATH
else
  LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH cmake -DCMAKE_BUILD_TYPE=Release $SCRIPT_PATH
fi

make $@

if [ -f "$BUILD_PATH/compile_commands.json" ]; then
  echo "Generated compile_commands.json (for code navigation) symlink next to build script"
  ln -sf "$BUILD_PATH/compile_commands.json" $SCRIPT_PATH/compile_commands.json
fi
