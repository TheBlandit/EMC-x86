#include <stdint.h>

// Hello
void println(uint32_t line, char* print);

void _start() {
    println(0, "Hello from 32-bit C running in protected mode");

    while (1) {
        asm volatile("hlt");
    }
}

void println(uint32_t line, char* print) {
    char volatile* vga = (char volatile*)(line * 160 + 0xb8000);
    while (*print != '\0') {
        *vga = *print;
        print++;
        vga += 2;
    }
}
