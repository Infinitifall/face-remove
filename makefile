CC=gcc
EMCC=emcc
WARN_FLAGS = -Wall -Wextra
DEBUG_FLAGS = -g
PERF_FLAGS = -O3 -march=native -flto
EM_FLAGS = 	-O3 -msimd128 -flto \
			-sFILESYSTEM=0 \
			-s EXPORT_ES6 \
			-s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'UTF8ToString', 'stringToNewUTF8']" \
			-s "EXPORTED_FUNCTIONS=['_malloc', '_free']"

SRC_DIR = src
BUILD_DIR = build
BUILD_DIR_WASM = html/build
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
WASM_SRCS = $(filter-out $(SRC_DIR)/file_io.c, $(wildcard $(SRC_DIR)/*.c))


.PHONY: all debug wasm clean


$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(BUILD_DIR_WASM))

all: native wasm

native: CFLAGS = $(WARN_FLAGS) $(PERF_FLAGS)
native: LDFLAGS = $(PERF_FLAGS)
native: face-remove

debug: CFLAGS = $(WARN_FLAGS) $(DEBUG_FLAGS)
debug: LDFLAGS =
debug: face-remove

wasm: $(WASM_SRCS)
	$(EMCC) $(EM_FLAGS) -o $(BUILD_DIR_WASM)/face-remove.js $^

face-remove: $(OBJS)
	$(CC) $(LDFLAGS) -o $(BUILD_DIR)/$@ $^ -lm

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	$(RM) -r $(BUILD_DIR)/*
	$(RM) -r $(BUILD_DIR_WASM)/*
