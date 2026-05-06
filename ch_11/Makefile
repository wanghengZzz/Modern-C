CC := gcc
OUT := bin
BIN := $(OUT)/main
SRC := $(wildcard src/*.c)
TARGET := challenge_11.c
VIPS_CFLAGS := $(shell pkg-config --cflags --libs vips)
CFLAGS := -Wall -Wextra
PIC_PATH := ./aniya.jpeg
PIXELS_DIFF := 5clear
OBJ := $(patsubst src/%.c, build/%.o, $(SRC)) \
	   $(patsubst %.c, build/%.o, $(TARGET))

VALGRIND_FALGS := \
 --tool=memcheck \
 --leak-check=full \
 --show-leak-kinds=all \
 --undef-value-errors=no \
 --log-file=valgrind_log
 
.PHONY: all clean run

all: $(BIN)

build:
	mkdir -p build

build/%.o: src/%.c | build
	@$(CC) $(CFLAGS) -c $< -o $@ $(VIPS_CFLAGS)

build/%.o: %.c | build
	@$(CC) $(CFLAGS) -c $< -o $@ $(VIPS_CFLAGS)

$(BIN): $(OBJ)
	mkdir -p $(OUT)
	@$(CC) $(OBJ) -o $(BIN) $(VIPS_CFLAGS)
	@echo "✅ Build complete: $(BIN)"

run: all
	@./$(BIN) $(PIC_PATH) $(PIXELS_DIFF)

clean:
	@rm -rf build $(OUT) valgrind_log
	@echo "🧹 Clean done."

format:
	find . -iname "*.c" -o -iname "*.h" | xargs clang-format -i -style=Google
	@echo "✨ All files formatted."

valgrind: $(BIN)
	valgrind $(VALGRIND_FALGS) $(BIN) $(PIC_PATH) $(PIXELS_DIFF)