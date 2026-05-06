CC := gcc
CFLAGS := -Wall -Wextra
OUT := bin
BIN := $(OUT)/main
SRC := $(wildcard src/*.c)
TARGET := challenge_18.c
OBJ := $(patsubst src/%.c, build/%.o, $(SRC)) \
	   $(patsubst %.c, build/%.o, $(TARGET))
NUM_THRDS := 4
DATA := ../data/mols.csv

VALGRIND_FALGS := \
	--tool=memcheck \
	--leak-check=full \
	--show-leak-kinds=all \
	--undef-value-errors=no \
	--log-file=valgrind_log \
	--max-threads=$(NUM_THRDS)


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
	@./$(BIN) $(NUM_THRDS) $(DATA)

clean:
	@rm -rf build $(OUT) valgrind_log
	@echo "🧹 Clean done."

format:
	find . -iname "*.c" -o -iname "*.h" | xargs clang-format -i -style=Google
	@echo "✨ All files formatted."

valgrind: $(BIN)
	valgrind $(VALGRIND_FALGS) $(BIN) $(NUM_THRDS) $(DATA)

