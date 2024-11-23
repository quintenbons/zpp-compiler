SCRIPT_PATH="$(cd -- "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd -P)"
export COMP_REPO_PATH=$(realpath $SCRIPT_PATH)
export COMP_BUILD_PATH="$COMP_REPO_PATH/cmake-build"
export COMP_BIN_PATH="$COMP_BUILD_PATH/bin"
export COMP_ARTIFACTS_PATH="$COMP_BUILD_PATH/.artifacts"

if [ ":$PATH:" != *":$COMP_BIN_PATH:"* ]; then
  export PATH="$COMP_BIN_PATH:$PATH"
fi

ccd() {
  cd $COMP_REPO_PATH
}

comp_build() {
  $COMP_REPO_PATH/build.sh $@
}

comp_setenv() {
  source $COMP_REPO_PATH/setenv.sh
}

comp_nasm() {
  nasm -f elf64 $@
}

_comp_build_nasm() {
  local nasm_file=$1
  mkdir -p $COMP_ARTIFACTS_PATH
  comp_nasm $nasm_file -o $COMP_ARTIFACTS_PATH/a.o
  LD_LIBRARY_PATH="" ld $COMP_ARTIFACTS_PATH/a.o -o $COMP_ARTIFACTS_PATH/a.out
  echo $COMP_ARTIFACTS_PATH/a.out
}

comp_build_nasm() {
  local nasm_file=$1
  local exe_path=$(_comp_build_nasm $1)
  mv $exe_path a.out
  echo "output: ./a.out" >&2
}

comp_run_nasm() {
  local nasm_file=$1
  local exe_path=$(_comp_build_nasm $nasm_file)
  $exe_path
}
