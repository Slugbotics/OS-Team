K_CC = i686-elf-gcc
K_CFLAGS = -T kernel/linker.ld -nostdlib -ffreestanding
K_SRC = $(wildcard kernel/*.s kernel/*.c)
K_TARGET = kernel.o

all: $(K_TARGET)

$(K_TARGET): $(K_SRC)
	$(K_CC) $^ -o $@ $(K_CFLAGS)

clean:
	rm *.o