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

__attribute__((interrupt))
static void default_handler(struct int_frame* frame) {
    char* string = "Unknown Interrupt";
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
            return;
        }

        bytes[i << 1] = *string;
        string++;
    }
}

__attribute__((interrupt))
static void int_handler(struct int_frame* frame) {
    char* string = "Interrupt 80h";
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
            return;
        }

        bytes[i << 1] = *string;
        string++;
    }
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
}
