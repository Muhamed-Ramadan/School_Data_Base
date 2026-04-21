# ─────────────────────────────────────────────────────────────────
# Makefile — School Management System
# ─────────────────────────────────────────────────────────────────
# Usage:
#   make              -> build the release executable
#   make test         -> build with -DTESTING and run all test cases
#   make clean        -> remove build artifacts
#
# Compiler: MinGW GCC on Windows  (works with Linux GCC unchanged)
# ─────────────────────────────────────────────────────────────────

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
SRC     = src/main.c src/school.c src/validation.c src/input.c src/display.c
TARGET  = school_management_system.exe
TEST_EXE= tests/test_build.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test:
	$(CC) $(CFLAGS) -DTESTING $(SRC) -o $(TEST_EXE)
	python tests/run_tests.py

clean:
	del $(TARGET) $(TEST_EXE) 2>nul || true
