#pragma once

#include <stdint.h>

char* u64_to_string(int64_t integer);
char* i64_to_string(int64_t integer);
char println(char* string);

__attribute__((no_caller_saved_registers))
uint8_t inb(uint16_t port);
__attribute__((no_caller_saved_registers))
void outb(uint16_t port, uint8_t value);
