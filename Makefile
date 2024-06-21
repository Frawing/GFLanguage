compile:
	python3 GFLanguage.py -i main.gfl -dt -dn -dct

test_asm:
#fasm program.asm
	nasm program.asm -f elf64 -o program.o
#gcc program.o -o PROGRAM
	ld -o PROGRAM program.o -dynamic-linker /lib64/ld-linux-x86-64.so.2 -L./raylib/ -lc -lraylib -lm

run:
	./PROGRAM