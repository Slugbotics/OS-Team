K_CC = i686-elf-gcc
K_CFLAGS = -m32 -T kernel/linker.ld -nostdlib -ffreestanding
K_SRC = $(wildcard kernel/*.s kernel/*.c)
K_TARGET = kernel.o
QEMU = qemu-system-x86_64

.PHONY: all qemu clean

all: $(K_TARGET)

$(K_TARGET): $(K_SRC)
	$(K_CC) $^ -o $@ $(K_CFLAGS)

qemu: $(K_TARGET)
	$(QEMU) -kernel $(K_TARGET)

clean:
	rm *.o