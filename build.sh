#!/bin/bash
SCRIPT_PATH="$(cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
if [ -z $ZPP_REPO_PATH ] ; then
  source $SCRIPT_PATH/setenv.sh
fi

if [ "$1" == "-c" ]; then
  echo "== Cleaning full $ZPP_BUILD_PATH =="
  rm -rf $ZPP_BUILD_PATH
  shift
fi

mkdir -p $ZPP_BUILD_PATH && cd $ZPP_BUILD_PATH

if [ "$1" == "-d" ]; then
  shift
  LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH cmake -DCMAKE_BUILD_TYPE=Debug $ZPP_REPO_PATH
else
  LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH cmake -DCMAKE_BUILD_TYPE=Release $ZPP_REPO_PATH
fi

make $@

if [ -f "$ZPP_BUILD_PATH/compile_commands.json" ]; then
  echo "Generated compile_commands.json (for code navigation) symlink next to build script"
  ln -sf "$ZPP_BUILD_PATH/compile_commands.json" $ZPP_REPO_PATH/compile_commands.json
fi
