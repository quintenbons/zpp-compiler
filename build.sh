SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
BUILD_PATH=./cmake-build

if [ "$1" == "-c" ]; then
  echo "== Cleaning full $BUILD_PATH =="
  rm -rf $BUILD_PATH
  shift
fi

mkdir -p $BUILD_PATH && cd $BUILD_PATH

if [ "$1" == "-d" ]; then
  shift
  LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH cmake -DCMAKE_BUILD_TYPE=Debug $SCRIPTPATH
else
  LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH cmake -DCMAKE_BUILD_TYPE=Release $SCRIPTPATH
fi

make $@
