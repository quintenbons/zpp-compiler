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
OUTPUT_EXTS = ["asm", "o", "out"]

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

def try_move_output(test_input, subprocess_result: subprocess.CompletedProcess[str]):
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

    # Write stdout, stderr, and return code
    (results_dir / f"{base_dir.stem}.stdout").write_text(subprocess_result.stdout or '')
    (results_dir / f"{base_dir.stem}.stderr").write_text(subprocess_result.stderr or '')
    (results_dir / f"{base_dir.stem}.status").write_text(str(subprocess_result.returncode))

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

    if result.returncode != 0: logging.debug(f"Compilation failed [{result.returncode}] for {test_input}")
    else: logging.debug(f"Compilation successful for {test_input}")

    return result

@pytest.mark.parametrize("test_file", CPP_TESTBASE.rglob("*.cpp"))
def test_compiler_output(test_file):
    logging.debug(f"Testing file: {test_file}")
    subprocess_result = run_compiler(test_file)
    moved_files = try_move_output(test_file, subprocess_result)
    assert len(get_outputs()) == 0, "Some outputs still exist"
    for file in moved_files:
        assert file.exists(), f"Output file {file} does not exist"

    expect_fail = FAILING_KEYWORD not in str(test_file)
    acceptable_returncodes = ACCEPTABLE_RETURNCODES if expect_fail else FAILING_RETURNCODES
    logging.debug(f"Outputs generated with return code [{subprocess_result.returncode}] allowed {acceptable_returncodes} (expect_fail={expect_fail})")
    assert subprocess_result.returncode in acceptable_returncodes, f"Unexpected return code [{subprocess_result.returncode}] expected_fail=[{expect_fail}]"

