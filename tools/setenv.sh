SCRIPT_PATH="$(cd -- "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd -P)"

export ZPP_REPO_PATH=$(realpath $SCRIPT_PATH/..)
export ZPP_BUILD_PATH="$ZPP_REPO_PATH/cmake-build"
export ZPP_CPP_TESTBASE="$ZPP_REPO_PATH/cpp_testbase"
export ZPP_ASM_TESTBASE="$ZPP_REPO_PATH/asm_testbase"
export ZPP_TEST_PATH="$ZPP_REPO_PATH/test"
export ZPP_BIN_PATH="$ZPP_BUILD_PATH/bin"
export ZPP_ARTIFACTS_PATH="$ZPP_BUILD_PATH/.artifacts"
export ZPP_REGRESSION_BASELINE_PATH="$ZPP_REPO_PATH/test/regression/baseline"
export ZPP_REGRESSION_RESULTS_PATH="$ZPP_REPO_PATH/test/regression/results"

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

zpp_test_cpp              run regression tests on c++ test base
zpp_test_cpp_debug        run regression tests on c++ test base with more verbose output
zpp_regression_diff       shows which files changed (dumps vimdiff command)
zpp_regression_fulldiff   shows which files changed (dumps vimdiff command)
zpp_regression_sync       syncs baseline based on current result
zpp_test_asm              run assembly tests
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

zpp_test_cpp() {
  echo "Running C++ tests..."
  pushd $ZPP_TEST_PATH
  local res_path=./regression/results/cpp_testbase
  if [ -d "$res_path" ] ; then
    rm -r $res_path
  fi
  poetry install
  poetry run pytest ./regression/cpp_testbase_test.py $@
  popd
}

zpp_test_cpp_debug() {
  zpp_test_cpp --log-cli-level=debug -v $@
}

zpp_regression_diff() {
  diff --exclude="*.o" --exclude="*.so" --exclude="*.out" -q -r $ZPP_REPO_PATH/test/regression/baseline/ $ZPP_REPO_PATH/test/regression/results/ | awk '{print "vimdiff " $2 " " $4}'
}

zpp_regression_fulldiff() {
  diff -q -r $ZPP_REPO_PATH/test/regression/baseline/ $ZPP_REPO_PATH/test/regression/results/ | awk '{print "vimdiff " $2 " " $4}'
}

zpp_regression_sync() {
  echo "Hard resetting baseline dir" >&2
  pushd $ZPP_REPO_PATH
  rm -r "$ZPP_REGRESSION_BASELINE_PATH"
  git checkout "$ZPP_REGRESSION_BASELINE_PATH"
  popd

  realdiffs=$(zpp_regression_diff)

  if [ -z "$realdiffs" ]; then
    if [ "$1" == "-f" ]; then
      rsync -a "$ZPP_REGRESSION_RESULTS_PATH/" "$ZPP_REGRESSION_BASELINE_PATH"
      echo "No real diffs were seen (possibly .o .so, .out)"
    else
      echo "No real diffs were seen (possibly .o .so, .out) if you want to force sync, use -f"
    fi
  else
    rsync -a "$ZPP_REGRESSION_RESULTS_PATH/" "$ZPP_REGRESSION_BASELINE_PATH"
    echo "Real diffs:"
    printf '%s\n' "$realdiffs"
  fi
}

zpp_test_asm() {
  echo "Running Assembly tests..."
  echo "Does not yet exist"
  # pushd $ZPP_ASM_TESTBASE
  # pytest ./regression/asm_testbase_test.py
  # popd
}
