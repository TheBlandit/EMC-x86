/// 64 bit kernel

#include <stdint.h>
#include "../include/interrupts.h"
#include "../include/utils.h"

void _start() {
    println("64-bit mode");

    for (uint64_t i = 0; i < 22; i++) {
        println(u64_to_string(1 << i));
    }

    create_idt();

    __asm__ volatile("int $0x80\n\t");
    println("Returned from interrupt");

    while (1) {
        __asm__ volatile("hlt\n\t");
    }
}
