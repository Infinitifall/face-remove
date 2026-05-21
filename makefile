CC=gcc
EMCC=emcc
WARN_FLAGS = -Wall -Wextra
DEBUG_FLAGS = -g
PERF_FLAGS = -O3 -ffast-math -march=native -flto

SRC_DIR = src
BUILD_DIR = build

.PHONY: all debug clean


$(shell mkdir -p $(BUILD_DIR))

all: CFLAGS = $(WARN_FLAGS) $(PERF_FLAGS)
all: LDFLAGS = $(PERF_FLAGS)
all: face-remove

debug: CFLAGS = $(WARN_FLAGS) $(DEBUG_FLAGS)
debug: LDFLAGS =
debug: face-remove


face-remove: $(BUILD_DIR)/face-remove.o $(BUILD_DIR)/cube_io.o $(BUILD_DIR)/file_io.o $(BUILD_DIR)/cube.o $(BUILD_DIR)/matrix.o $(BUILD_DIR)/vector.o
	$(CC) $(LDFLAGS) -o $(BUILD_DIR)/$@ $^ -lm


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	$(RM) -r $(BUILD_DIR)/*
