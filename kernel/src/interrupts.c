#include "../include/shared_constants.h"
#include "../include/utils.h"
#include <inttypes.h>
#include <stdint.h>

struct int_desc {
    uint16_t offset_1;        // offset bits 0..15
    uint16_t selector;        // a code segment selector in GDT or LDT
    uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t  type_attributes; // gate type, dpl, and p fields
    uint16_t offset_2;        // offset bits 16..31
    uint32_t offset_3;        // offset bits 32..63
    uint32_t zero;            // reserved
};

struct int_frame {
    uintptr_t ip;
    uintptr_t cs;
    uintptr_t flags;
    uintptr_t sp;
    uintptr_t ss;
};

__attribute__((no_caller_saved_registers))
static void int_println(char* string) {
    uint64_t* start = (uint64_t*)VGA_TEXT_BUFFER;
    const uintptr_t REP = ((VGA_HEIGHT - 2) * VGA_WIDTH * 2) / sizeof(uint64_t);
    for (uintptr_t i = 20; i < REP; i++) {
        start[i] = start[i + 20];
    }

    char* bytes = (char*)(start + REP);
    for (uintptr_t i = 0; i < VGA_WIDTH; i++) {
        bytes[i << 1] = 0;
    }

    for (uintptr_t i = 0; i < VGA_WIDTH; i++) {
        if (*string == '\0') {
            return;
        }

        bytes[i << 1] = *string;
        string++;
    }
}

__attribute__((no_caller_saved_registers))
static void int_print_status(char* string) {
    uint16_t* start = (uint16_t*)VGA_TEXT_BUFFER;

    for (uintptr_t i = 0; i < VGA_WIDTH; i++) {
        start[i] = 0x4F20;
    }

    char* bytes = (char*)VGA_TEXT_BUFFER;
    for (uintptr_t i = 0; i < VGA_WIDTH; i++) {
        if (*string == '\0') {
            return;
        }

        bytes[i << 1] = *string;
        string++;
    }
}

__attribute__((interrupt))
static void default_handler(struct int_frame* frame) {
    int_print_status("Unknown Interrupt");
}

__attribute__((no_caller_saved_registers))
static char hex(uint8_t nibble) {
    if (nibble > 9) {
        return nibble + 'A' - 10;
    } else {
        return nibble + '0';
    }
}

static char scancode_map[256] = {
    // 00
    '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\0', '\0',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '#', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '\0', '\0', '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
    // 40
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\\', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    // 80
    '\0', '\0', '!', '"', '\0', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\0', '\0',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '@', '\0', '\0', '~', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', '\0', '\0', '\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
    // C0
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '|', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
};

__attribute__((interrupt))
static void keyboard_handler(struct int_frame* frame) {
    uint8_t scancode = inb(0x60);

    static uintptr_t char_index = 0;
    static char shift = 0;

    uint16_t* vga = (uint16_t*)(VGA_TEXT_BUFFER + VGA_WIDTH * 2 * (VGA_HEIGHT - 1));
    // for (uintptr_t i = 0; i < VGA_WIDTH; i++) {
    //     vga[i] = 0x1F20;
    // }

    if (scancode < 0x80) {
        scancode |= shift;
        char map = scancode_map[scancode];
        if (map == '\0') {
            if (scancode == 0x2A) {
                shift = 0x80;
            } else if (scancode == 0x0E) {
                if (char_index > 0) {
                    char_index--;
                    *(char*)(vga + char_index) = 0x20;
                }
            } else {
                char* string = "AA";
                string[0] = hex(scancode >> 4);
                string[1] = hex(scancode & 0xF);
                int_print_status(string);
            }
        } else {
            if (char_index < VGA_WIDTH) {
                *(char*)(vga + char_index) = map;
                char_index++;
            }
        }
    } else {
        if (scancode == 0xAA) {
            shift = 0;
        }
    }

    outb(0x20, 0x20);
}

__attribute__((interrupt))
static void int_handler(struct int_frame* frame) {
    int_print_status("Interrupt 80h");
}

static void create_entry(uint8_t entry, void (*ptr)(struct int_frame*), uint8_t type_attributes) {
    struct int_desc desc;
    desc.zero = 0;
    desc.offset_1 = (uint16_t)(uintptr_t)ptr;
    desc.offset_2 = (uint16_t)((uintptr_t)ptr >> 16);
    desc.offset_3 = (uint32_t)((uintptr_t)ptr >> 32);
    desc.ist = 0;
    desc.type_attributes = type_attributes;
    desc.selector = 0x08;
    ((struct int_desc*)IDT_BASE_ADDRESS)[entry] = desc;
}

void create_idt() {
    // zero out all interrupt descriptors
    for (uint16_t i = 0; i < 256; i++) {
        create_entry((uint8_t)i, &default_handler, 0xEE);
    }

    create_entry(0x80, &int_handler, 0xEE);
    create_entry(0x21, &keyboard_handler, 0xEE);

    uint16_t* lidt = (uint16_t*)(IDT_BASE_ADDRESS - 10);
    *lidt = 256 * sizeof(struct int_desc) - 1;
    *(uint64_t*)(IDT_BASE_ADDRESS - 8) = IDT_BASE_ADDRESS;

    __asm__ volatile (
        "lidt %0\n\t"
        "sti\n\t"
        :
        : "m" (*lidt)
        : "memory"
    );

    // Enable PIC
    outb(0x20, 0x11);  // Initialize the command port
    outb(0x21, 0x20);  // Set vector offset (IRQ0-IRQ7)
    outb(0x21, 0x04);  // Set cascading (IRQ2)
    outb(0x21, 0x01);  // Set 8086 mode
    outb(0x21, 0xFF);  // Mask all interrupts initially

    outb(0x21, inb(0x21) & 0xFD);  // Unmask IRQ1 (keyboard)
}
