# Cross-platform Makefile: works on bash (*nix) and on Windows from PowerShell with mingw32-make
# Usage (Linux/macOS):
#   make
#   make test
#   make run
# Usage (Windows PowerShell):
#   mingw32-make CC=gcc
#   mingw32-make CC=gcc test
#   mingw32-make CC=gcc run

# ----- toolchain & flags -----
CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wno-unused-parameter
INCLUDES := -Iinclude -Itests/vendor/unity
DEFINES := -DIMU_DRIVER_NO_DEFAULT_I2C

SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build

APP_SOURCES  := $(SRC_DIR)/main.c $(SRC_DIR)/imu_driver.c $(SRC_DIR)/mock_i2c.c
TEST_SOURCES := $(TEST_DIR)/test_imu.c $(SRC_DIR)/imu_driver.c $(SRC_DIR)/mock_i2c.c $(TEST_DIR)/vendor/unity/unity.c

APP_OBJS  := $(patsubst %.c,$(BUILD_DIR)/%.o,$(APP_SOURCES))
TEST_OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TEST_SOURCES))

# ----- platform detection / helpers -----
ifeq ($(OS),Windows_NT)
  EXE := .exe
  # cmd.exe-friendly helpers
  MKDIR_P = if not exist "$(1)" mkdir "$(1)"
  RM_RF   = rmdir /S /Q
else
  EXE :=
  MKDIR_P = mkdir -p "$(1)"
  RM_RF   = rm -rf
endif

BIN_APP  := $(BUILD_DIR)/app$(EXE)
BIN_TEST := $(BUILD_DIR)/unit_tests$(EXE)

# ----- default -----
all: app tests

# ----- link targets -----
app: $(BIN_APP)
tests: $(BIN_TEST)

$(BIN_APP): $(APP_OBJS)
	$(call MKDIR_P,$(BUILD_DIR))
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) $^ -o $@

$(BIN_TEST): $(TEST_OBJS)
	$(call MKDIR_P,$(BUILD_DIR))
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) $^ -o $@

# ----- compile rule (auto-create obj dirs) -----
$(BUILD_DIR)/%.o: %.c
	$(call MKDIR_P,$(@D))
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

# ----- convenience -----
test: tests
	$(BIN_TEST)

run: app
	$(BIN_APP)

clean:
	-$(RM_RF) $(BUILD_DIR)

.PHONY: all app tests test run clean
