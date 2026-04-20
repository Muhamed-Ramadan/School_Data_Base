"""
run_tests.py — Automated test runner for School Management System
─────────────────────────────────────────────────────────────────
Usage:
    python run_tests.py

Requirements:
    - MinGW gcc available in PATH
    - Run from the repository root directory

What it does:
    1. Compiles the project with -DTESTING flag
    2. Runs each test input file through the compiled binary
    3. Saves output to tests/output/
    4. If a matching expected file exists in tests/expected/,
       compares and reports PASS or FAIL
    5. Prints a summary at the end
"""

import subprocess
import os
import sys

# ── Configuration ─────────────────────────────────────────────────
SRC_FILES   = ["src/main.c", "src/school.c"]
OUTPUT_EXE  = "tests/test_build.exe"
INPUT_DIR   = "tests/input"
OUTPUT_DIR  = "tests/output"
EXPECTED_DIR= "tests/expected"
COMPILER    = "gcc"
# ──────────────────────────────────────────────────────────────────

os.makedirs(OUTPUT_DIR, exist_ok=True)

# ── Step 1: Compile ───────────────────────────────────────────────
print("Compiling with -DTESTING...")
compile_cmd = [COMPILER] + SRC_FILES + ["-DTESTING", "-o", OUTPUT_EXE]
result = subprocess.run(compile_cmd, capture_output=True, text=True)

if result.returncode != 0:
    print("COMPILE ERROR:")
    print(result.stderr)
    sys.exit(1)

print("Compilation successful.\n")

# ── Step 2: Run tests ─────────────────────────────────────────────
test_files = sorted(f for f in os.listdir(INPUT_DIR) if f.endswith(".txt"))

if not test_files:
    print("No test files found in", INPUT_DIR)
    sys.exit(0)

passed = 0
failed = 0
skipped = 0
results = []

for test_file in test_files:
    test_name   = test_file.replace(".txt", "")
    input_path  = os.path.join(INPUT_DIR,    test_file)
    output_path = os.path.join(OUTPUT_DIR,   test_file)
    expected_path = os.path.join(EXPECTED_DIR, test_file)

    # Run the binary with redirected input
    with open(input_path, "r") as fin:
        run_result = subprocess.run(
            [OUTPUT_EXE],
            stdin=fin,
            capture_output=True,
            text=True,
            timeout=10
        )

    # Save output
    with open(output_path, "w") as fout:
        fout.write(run_result.stdout)

    # Compare with expected if it exists
    if os.path.exists(expected_path):
        with open(expected_path, "r") as fexp:
            expected = fexp.read()
        if run_result.stdout.strip() == expected.strip():
            status = "PASS"
            passed += 1
        else:
            status = "FAIL"
            failed += 1
    else:
        status = "SAVED"   # output saved, no expected file to compare against
        skipped += 1

    results.append((test_name, status))
    print(f"  [{status:6}]  {test_name}")

# ── Step 3: Summary ───────────────────────────────────────────────
print()
print("─" * 50)
print(f"  Results:  {passed} passed  |  {failed} failed  |  {skipped} saved (no expected)")
print("─" * 50)

if failed > 0:
    print("\nFailed tests — compare files in tests/output/ vs tests/expected/")
    for name, status in results:
        if status == "FAIL":
            print(f"  - {name}")

print("\nAll outputs saved to:", OUTPUT_DIR)

# Clean up test binary
if os.path.exists(OUTPUT_EXE):
    os.remove(OUTPUT_EXE)
