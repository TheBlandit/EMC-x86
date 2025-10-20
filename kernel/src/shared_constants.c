#include "../include/shared_constants.h"
#include <stdint.h>

const uintptr_t IDENTITY_BASE_ADDRESS = 1 << 20;
const uintptr_t IDT_BASE_ADDRESS = 0x1750000;

const uintptr_t VGA_TEXT_BUFFER = 0xB8000;
const uintptr_t VGA_WIDTH = 80;
const uintptr_t VGA_HEIGHT = 25;

