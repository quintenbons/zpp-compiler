SCRIPT_PATH="$(cd -- "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd -P)"

export ZPP_REPO_PATH=$(realpath $SCRIPT_PATH/..)
export ZPP_BUILD_PATH="$ZPP_REPO_PATH/cmake-build"
export ZPP_BIN_PATH="$ZPP_BUILD_PATH/bin"
export ZPP_ARTIFACTS_PATH="$ZPP_BUILD_PATH/.artifacts"

if [ ":$PATH:" != *":$ZPP_BIN_PATH:"* ]; then
  export PATH="$ZPP_BIN_PATH:$PATH"
fi

zpp_commands() {
  echo """##############################
### ZPP - Zero C++ Compiler ##
##############################

cdd                   cd to repo root
zpp_setenv            reset zpp environment
zpp_clean             clean build directories
zpp_build             build all targets (-c/-d)
zpp_nasm              nasm with recommended flags
zpp_assemble_binary   assemble and link .asm files to ./a.out
zpp_run_nasm          assemble, link and execute .asm files
"""
}

ccd() {
  cd $ZPP_REPO_PATH
}

zpp_setenv() {
  source $ZPP_REPO_PATH/setenv.sh
}

zpp_clean() {
  rm -r $ZPP_BUILD_PATH
}

zpp_build() {
  $ZPP_REPO_PATH/build.sh $@
}

zpp_setenv() {
  source $ZPP_REPO_PATH/setenv.sh
}

zpp_nasm() {
  nasm -f elf64 $@
}

_zpp_build_nasm() {
  local nasm_file=$1
  mkdir -p $ZPP_ARTIFACTS_PATH
  zpp_nasm $nasm_file -o $ZPP_ARTIFACTS_PATH/a.o
  LD_LIBRARY_PATH="" ld $ZPP_ARTIFACTS_PATH/a.o -o $ZPP_ARTIFACTS_PATH/a.out
  echo $ZPP_ARTIFACTS_PATH/a.out
}

zpp_assemble_binary() {
  local nasm_file=$1
  local exe_path=$(_zpp_build_nasm $1)
  mv $exe_path a.out
  echo "output: ./a.out"
}

zpp_run_nasm() {
  local nasm_file=$1
  local exe_path=$(_zpp_build_nasm $nasm_file)
  $exe_path
}
