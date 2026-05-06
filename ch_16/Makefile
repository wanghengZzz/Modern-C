CC := gcc
CFLAGS := -Wall -Wextra
OUT := bin
BIN := $(OUT)/main
SRC := $(wildcard src/*.c)
TARGET := challenge_16.c
STR_MAX_LEN := 100
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
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BIN): $(OBJ)
	mkdir -p $(OUT)
	$(CC) $(OBJ) -o $(BIN)
	@echo "✅ Build complete: $(BIN)"

run: all
	@./$(BIN) $(STR_MAX_LEN)

clean:
	@rm -rf build $(OUT) valgrind_log
	@echo "🧹 Clean done."

format:
	find . -iname "*.c" -o -iname "*.h" | xargs clang-format -i -style=Google
	@echo "✨ All files formatted."

valgrind: $(BIN)
	valgrind $(VALGRIND_FALGS) $(BIN)

