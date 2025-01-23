import sys
import os
import subprocess
import pytest
import logging
import shutil
from pathlib import Path

FAILING_KEYWORD = "failing"
FAILING_RETURNCODES = [1, 3]
ACCEPTABLE_RETURNCODES = [0, 1, 3]
OUT = "out"
OUTPUT_EXTS = ["asm", "o", OUT]

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from config import *

def getEnv(var)->str:
    value = os.environ.get(var)
    assert value is not None, f"{var} not set. Please source setenv.sh before launching this script"
    return value

# Directory for output results
CPP_TESTBASE = Path(getEnv(ZPP_TESTBASE_ENV))
RESULTS_DIR = Path(__file__).parent / "results" / "cpp_testbase"
RESULTS_DIR.mkdir(parents=True, exist_ok=True)

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logging.info(f"Using test base directory: {CPP_TESTBASE}")
logging.info(f"Results will be stored in: {RESULTS_DIR}")

def try_move_file(src, dest):
    try:
        shutil.move(src, dest)
        return True
    except FileNotFoundError:
        return False

import subprocess
from pathlib import Path

def get_outputs() -> list[Path]:
    outputs = [Path(".") / f"a.{ext}" for ext in OUTPUT_EXTS]
    return [out for out in outputs if out.exists()]

def get_executable() -> Path:
    return f"./a.{OUT}"

def try_move_output(test_input, compile_process_result: subprocess.CompletedProcess[str], execution_process_result: subprocess.CompletedProcess[str]):
    base_dir = Path(test_input).relative_to(CPP_TESTBASE)  # get relative path
    results_dir = RESULTS_DIR / base_dir.parent  # use the parent directory of the relative path

    # Ensure the target directory exists
    results_dir.mkdir(parents=True, exist_ok=True)

    result_files = []

    # Move and rename result files based on extensions
    outputs = get_outputs()
    for out in outputs:
        ext = out.suffix
        src = Path(".") / f"a{ext}"
        dst = results_dir / f"{base_dir.stem}{ext}"
        logging.debug(f"Moving {src} to {dst}")
        assert try_move_file(src, dst), f"Could not move {src} to {dst}"
        if src.exists() and src.rename(dst):
            result_files.append(dst)

    # Remove the first line of compile_process_result.stdout (it contains the file name)
    compile_process_result.stdout = compile_process_result.stdout.split("\n", 1)[1]

    # Write stdout, stderr, and return code
    (results_dir / f"{base_dir.stem}-compile.stdout").write_text(compile_process_result.stdout or '')
    (results_dir / f"{base_dir.stem}-compile.stderr").write_text(compile_process_result.stderr or '')
    (results_dir / f"{base_dir.stem}-compile.status").write_text(str(compile_process_result.returncode))

    if execution_process_result is not None:
        (results_dir / f"{base_dir.stem}-exec.stdout").write_text(execution_process_result.stdout or '')
        (results_dir / f"{base_dir.stem}-exec.stderr").write_text(execution_process_result.stderr or '')
        (results_dir / f"{base_dir.stem}-exec.status").write_text(str(execution_process_result.returncode))

    return result_files

def run_compiler(test_input):
    # cmd = ["z++", str(test_input), "-o", str(output_file)] # once issue #12 is done
    cmd = ["z++", "-d", str(test_input)]

    logging.debug(f"Compiling file {test_input} into [a.out]")
    logging.debug(f"Command [{' '.join(cmd)}]")
    result = subprocess.run(cmd, capture_output=True, text=True)

    # Log stdout and stderr
    if result.stdout: logging.debug(f"Compiler stdout for {test_input}:\n{result.stdout}")
    if result.stderr: logging.debug(f"Compiler stderr for {test_input}:\n{result.stderr}")

    executed_successfully = result.returncode == 0
    if not executed_successfully: logging.debug(f"Compilation failed [{result.returncode}] for {test_input}")
    else: logging.debug(f"Compilation successful for {test_input}")

    return result, executed_successfully

def run_program(test_input):
    cmd = [str(get_executable())]

    logging.debug(f"Running file {test_input}")
    logging.debug(f"Command [{' '.join(cmd)}]")
    result = subprocess.run(cmd, capture_output=True, text=True)

    # Log stdout and stderr
    if result.stdout: logging.debug(f"Program stdout for {test_input}:\n{result.stdout}")
    if result.stderr: logging.debug(f"Program stderr for {test_input}:\n{result.stderr}")

    if result.returncode != 0: logging.debug(f"Program failed [{result.returncode}] for {test_input}")
    else: logging.debug(f"Successful execution for {test_input}")

    return result

@pytest.mark.parametrize(
    "test_file",
    CPP_TESTBASE.rglob("*.cpp"),
    ids=[str(test_file.relative_to(CPP_TESTBASE)) for test_file in CPP_TESTBASE.rglob("*.cpp")]  # Generating IDs based on relative paths
)
def test_compiler_output(test_file):
    logging.debug(f"Testing file: {test_file}")
    compile_process_result, has_compiled = run_compiler(test_file)
    execution_process_result = None
    if has_compiled:
        execution_process_result = run_program(test_file)
    moved_files = try_move_output(test_file, compile_process_result, execution_process_result)
    assert len(get_outputs()) == 0, "Some outputs still exist"
    for file in moved_files:
        assert file.exists(), f"Output file {file} does not exist"

    expect_fail = FAILING_KEYWORD not in str(test_file)
    acceptable_returncodes = ACCEPTABLE_RETURNCODES if expect_fail else FAILING_RETURNCODES
    logging.debug(f"Outputs generated with return code [{compile_process_result.returncode}] allowed {acceptable_returncodes} (expect_fail={expect_fail})")
    assert compile_process_result.returncode in acceptable_returncodes, f"Unexpected return code [{compile_process_result.returncode}] expected_fail=[{expect_fail}]"

