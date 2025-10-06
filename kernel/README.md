### How the kernel works

It loads the asm file at 0x7c00
It is padded to 512 bytes (a sector) and is ended with a magic number (0xAA55)
The asm file reads sectors from the c kernel into a known address (where _start in c is located)
The asm file sets up a gdt with 4 segment descriptors: null (gdt location and limit), code, data and a blank one
The asm file enables CR0.PE, sets all segment registers to 0x10 (data segment) and esp then far jumps to 8h:1000h (_start in C)
The C files are compiled with minimal symbols and to expect code to start at 0x1000

# Memory useage
0800:081F GDT
1000:1FFF 32bit C
2000:2FFF 64bit C
