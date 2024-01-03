<<<<<<< HEAD
all: clean compile clear run

compile:
#cmake --build build/
	g++ -fpermissive scr/main.cpp -o COMPILER -std=c++20

run:
	./COMPILER
=======
ARG = ./GFL/main.gfl

all: clean compile clear run

compile:
	cmake --build build/

run:
	./build/GCompiler ./GFL/main.gfl
>>>>>>> a3f0de8441e8d9e3af138c650e042a61a8dc3bc0
	nasm -felf64 out.asm
	ld -o PROGRAM out.o

clear:
	clear

clean:
	rm -f ./PROGRAM
	rm -f out.o
	rm -f out.asm

.PHONY: