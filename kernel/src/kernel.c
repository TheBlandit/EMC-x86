/// 64 bit kernel

#include <stdint.h>
#include "../include/interrupts.h"
#include "../include/utils.h"
#include "../include/shared_constants.h"

void _start() {
    // Create enterline
    uint16_t* vga = (uint16_t*)(VGA_TEXT_BUFFER + VGA_WIDTH * 2 * (VGA_HEIGHT - 1));
    for (uintptr_t i = 0; i < VGA_WIDTH; i++) {
        vga[i] = 0x1F20;
    }

    println("64-bit mode");

    for (uint64_t i = 0; i < 22; i++) {
        println(u64_to_string(1 << i));
    }

    create_idt();

    __asm__ volatile("int $0x80\n\t");

    while (1) {
        __asm__ volatile("hlt\n\t");
    }
}
