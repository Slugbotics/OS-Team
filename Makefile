all:
	i686-elf-gcc start.s main.c -o kernel.o -T linker.ld -nostdlib -ffreestanding