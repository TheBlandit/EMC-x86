#include "../include/shared_constants.h"
#include <stdint.h>

char shared[64] = {};

void internal_u64_to_string(uint64_t integer, char* ptr) {
    // Store the string backwards
    uintptr_t length = 0;
    if (integer == 0) {
        ptr[0] = '0';
        length++;
    } else {
        while (integer > 0) {
            ptr[length] = (integer % 10) + '0';
            length++;
            integer = integer / 10;
        }
    }

    // Reverse the string
    for (uintptr_t i = 0; i < (length >> 1); i++) {
        uintptr_t j = length - i - 1;
        ptr[i] ^= ptr[j];
        ptr[j] ^= ptr[i];
        ptr[i] ^= ptr[j];
    }

    // Add a null terminator
    ptr[length] = 0;
}

char* u64_to_string(uint64_t integer) {
    internal_u64_to_string(integer, shared);
    return shared;
}

char* i64_to_string(int64_t integer) {
    if (integer < 0) {
        shared[0] = '-';
        internal_u64_to_string((uint64_t)-integer, shared + 1);
    } else {
        internal_u64_to_string((uint64_t)integer, shared);
    }

    return shared;
}

/// If it cannot print the entire string, it returns 1 else it returns 0
char println(char* string) {
    uint64_t* start = (uint64_t*)VGA_TEXT_BUFFER;
    const uintptr_t REP = ((VGA_HEIGHT - 1) * VGA_WIDTH * 2) / sizeof(uintptr_t);
    for (uint64_t i = 0; i < REP; i++) {
        start[i] = start[i + 20]; // 20 = 160/8
    }

    start += REP;
    char* bytes = (char*)start;
    for (uint64_t i = 0; i < VGA_WIDTH; i++) {
        bytes[i << 1] = 0;
    }

    for (uint64_t i = 0; i < VGA_WIDTH; i++) {
        if (*string == '\0') {
            return 0;
        }

        bytes[i << 1] = *string;
        string++;
    }

    return *string != '\0';
}

__attribute__((no_caller_saved_registers))
uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );
    return value;
}

__attribute__((no_caller_saved_registers))
void outb(uint16_t port, uint8_t value) {
    asm volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}
