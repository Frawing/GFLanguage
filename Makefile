ARG = ./GFL/main.gfl

all: clean compile clear run

compile:
	cmake --build build/

run:
	./build/GCompiler ./GFL/main.gfl
	nasm -felf64 out.asm
	ld -o PROGRAM out.o

clear:
	clear

clean:
	rm -f ./PROGRAM
	rm -f out.o
	rm -f out.asm

.PHONY: