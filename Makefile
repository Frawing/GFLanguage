all: clean compile clear run

compile:
	g++ -fpermissive scr/main.cpp -o COMPILER -std=c++20

run:
	./COMPILER
	nasm -felf64 out.asm
	ld -o PROGRAM out.o

clear:
	clear

clean:
	rm -f ./PROGRAM
	rm -f out.o
	rm -f out.asm

.PHONY: