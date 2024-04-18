#include <stdarg.h>
#include <stdbool.h>
#include "multiboot.h"

extern char* kernel_start;
extern char* kernel_end;

char* vga_base = (char*)0x000b8000;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

unsigned int cursor_x = 0;
unsigned int cursor_y = 0;

static inline void outb(unsigned short port, char val)
{
    __asm__ ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline char inb(unsigned short port)
{
    char ret;
    __asm__ ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

void enable_cursor(char cursor_start, char cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void update_cursor()
{
	unsigned short pos = cursor_y * 80 + cursor_x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (char) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (char) ((pos >> 8) & 0xFF));
}

void vga_put_char(unsigned int x, unsigned int y, char ch) {
    unsigned int pos = (y * VGA_WIDTH + x) * 2;
    vga_base[pos] = ch;
}

void vga_clear() {
    for(int y = 0; y < VGA_HEIGHT; y++) {
        for(int x = 0; x < VGA_WIDTH; x++) {
            vga_put_char(x, y, ' ');
        }
    }
}

void vga_putc(char ch) {
    if(ch == '\n') {
        cursor_x = 0;
        cursor_y += 1;
    } else if(ch == '\t') {
        cursor_x += 8;
    } else {
        vga_put_char(cursor_x, cursor_y, ch);
        cursor_x += 1;
    }
    if(cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y += 1;
    }
    update_cursor();
}

void _k_print_int(unsigned long n, bool is_signed, int radix, bool captial, void (*putc_ptr)(char)) {
    char* digits;
    if(captial) {
        digits = "0123456789ABCDEF";
    }
    else {
        digits = "0123456789abcdef";
    }
    if(is_signed) {
        if((long long)n < 0) {
            putc_ptr('-');
            n = (unsigned long long)(-((long long)n));
        }
    }
    int multiplier = 1;
    while(n / multiplier >= radix) {
        multiplier *= radix;
    }
    while(multiplier > 0) {
        putc_ptr(digits[(n / multiplier) % radix]);
        multiplier /= radix;
    }
}

void _k_common_printf(char *fmt, va_list *args, void (*putc_ptr)(char)) {
    int i = 0;
    while(fmt[i] != '\0') {
        if(fmt[i] == '%' && fmt[i + 1] != '\0') {
            i++;
            bool is_long = false;
            bool is_long_long = false;
            if(fmt[i] == 'l' && fmt[i + 1] != '\0') {
                i++;
                if(fmt[i] == 'l' && fmt[i + 1] != '\0') {
                    is_long_long = true;
                    i++;
                } else {
                    is_long = true;
                }
            }
            switch (fmt[i])
            {
            case 's':
            {
                char *str = va_arg(*args, char*);
                int j = 0;
                while(str[j] != '\0') {
                    putc_ptr(str[j]);
                    j++;
                }
                break;
            }
            case 'd':
            case 'i':
                if(is_long_long) {
                    _k_print_int(va_arg(*args, long long int), true, 10, false, putc_ptr);
                } else if(is_long) {
                    _k_print_int(va_arg(*args, long int), true, 10, false, putc_ptr);
                } else {
                    _k_print_int(va_arg(*args, int), true, 10, false, putc_ptr);
                }
                break;
            case 'x':
                if(is_long_long) {
                    _k_print_int(va_arg(*args, unsigned long long int), false, 16, false, putc_ptr);
                } else if(is_long) {
                    _k_print_int(va_arg(*args, unsigned long int), false, 16, false, putc_ptr);
                } else {
                    _k_print_int(va_arg(*args, unsigned int), false, 16, false, putc_ptr);
                }
                break;
            case 'X':
                if(is_long_long) {
                    _k_print_int(va_arg(*args, unsigned long long int), false, 16, true, putc_ptr);
                } else if(is_long) {
                    _k_print_int(va_arg(*args, unsigned long int), false, 16, true, putc_ptr);
                } else {
                    _k_print_int(va_arg(*args, unsigned int), false, 16, true, putc_ptr);
                }
                break;
            case 'o':
                if(is_long_long) {
                    _k_print_int(va_arg(*args, unsigned long long int), false, 8, false, putc_ptr);
                } else if(is_long) {
                    _k_print_int(va_arg(*args, unsigned long int), false, 8, false, putc_ptr);
                } else {
                    _k_print_int(va_arg(*args, unsigned int), false, 8, false, putc_ptr);
                }
                break;
            case 'p':
                _k_print_int((unsigned long)va_arg(*args, void*), false, 16, true, putc_ptr);
                break;
            default:
                break;
            }
        }
        else {
            putc_ptr(fmt[i]);
        }
        i++;
    }
}

void k_printf(char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    _k_common_printf(fmt, &list, &vga_putc);
    va_end(list);
}

void kernel_main(multiboot_info_t *boot_info) {
    enable_cursor(0, 15);
    update_cursor();
    vga_clear();

    k_printf("Kernel start: 0x%X\n", &kernel_start);
    k_printf("Kernel end: 0x%X\n", &kernel_end);

    multiboot_memory_map_t *mmap = (multiboot_memory_map_t*)(boot_info->mmap_addr);
    k_printf("\nDetected physical memory blocks:\n");
    while(mmap < (multiboot_memory_map_t*)(boot_info->mmap_addr + boot_info->mmap_length)) {
        char* type_str = "Unknown";
        switch (mmap->type)
        {
        case MULTIBOOT_MEMORY_AVAILABLE:        type_str = "Available"; break;
        case MULTIBOOT_MEMORY_RESERVED:         type_str = "Reserved"; break;
        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE: type_str = "ACPI Reclaimable"; break;
        case MULTIBOOT_MEMORY_NVS:              type_str = "NVS"; break;
        case MULTIBOOT_MEMORY_BADRAM:           type_str = "Bad RAM"; break;
        break;
        default:
            break;
        }
        k_printf("    Base: 0x%llX, Length: 0x%llX, Type: %s\n", mmap->addr, mmap->len, type_str);
        mmap = (multiboot_memory_map_t*)((unsigned long)mmap + mmap->size + sizeof(mmap->size));
    }
}
