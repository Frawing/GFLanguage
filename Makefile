CC = g++

SCR = ./scr
BUILD = ./build

SOURCE = $(SCR)/main.cpp $(SCR)/include/*.cpp
COMPILER_PROGRAM = $(BUILD)/GCOMPILER

FLAGS = -std=c++20 -fpermissive

ASM = $(BUILD)/out.asm
OUT = $(BUILD)/out.o
GFL_PROGRAM = $(BUILD)/PROGRAM

ARGS = -i ./GFL/main.gfl -op ./build/ -dt true

all: clean compile clear run_compiler assemble

compile:
	$(CC) -g $(FLAGS) $(SOURCE) -o $(COMPILER_PROGRAM)

run_compiler:
	$(COMPILER_PROGRAM) $(ARGS)

assemble:
	nasm -felf64 $(ASM)
	ld -o $(GFL_PROGRAM) $(OUT)
clear:
	clear

clean:
	rm -f $(GFL_PROGRAM)
	rm -f $(OUT)
	rm -f $(ASM)

clean_compiler:
	rm -f $(OUTPUT)

.PHONY: