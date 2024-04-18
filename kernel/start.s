.set ALIGN,    1 << 0           # align loaded modules on page boundaries
.set MEMINFO,  1 << 1           # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
.skip 16384
stack_top:

.section .text
.global _start
_start:
    mov $stack_top, %esp    # Set top of stack
    push %ebx               # Provide pointer to boot info as argument to kernel
    call kernel_main
    cli
    hlt
    1:
    jmp 1b
