#include <stdint.h>
#include "identity.h"

// Prints a new line to the vga text buffer
void println(char* print);
uint32_t check_cpuid(void);

void _start() {
    println("Hello from 32-bit C running in protected mode");

    char* cpuid_str = "Does not suppport CPUID";
    if (check_cpuid()) {
        cpuid_str = "Suppports CPUID";
    }

    println(cpuid_str);

    paging_create();

    uint32_t cr3 = CR3;

    __asm__ volatile (
        // Enable PAE
        "mov %%cr4, %%eax\n\t"
        "or $0b100000, %%eax\n\t"
        "mov %%eax, %%cr4\n\t"
        // Put paging base into cr3
        "mov %0, %%eax\n\t"
        "mov %%eax, %%cr3\n\t"
        :
        : "r" (cr3)
        : "eax", "memory"
    );

    println("Loaded CR3");

    __asm__ volatile (
        // Enable IA32_EFER.LME
        "mov $0xC0000080, %%ecx\n\t"
        "rdmsr\n\t"
        "or $0x100, %%eax\n\t"
        "wrmsr\n\t"
        :
        :
        : "eax", "ecx", "edx"
    );

    println("Enabled LM");

    __asm__ volatile (
        // Enable Compatability mode (enables paging)
        "mov %%cr0, %%eax\n\t"
        "or $0x8000, %%eax\n\t"
        "mov %%eax, %%cr0\n\t"
        :
        :
        : "eax"
    );

    println("Enabled Compatibility");

    // Clear D/B for segment descriptors and set L
    uint64_t* segments = (uint64_t*)0x0808;
    const uint64_t SEG_FLAGS = ((uint64_t)1 << 54) | ((uint64_t)1 << 53);
    *segments ^= SEG_FLAGS;

    println("Idk");
}

void println(char* print) {
    static uint32_t line = 1;
    char volatile* vga = (char volatile*)(line * 160 + 0xb8000);
    while (*print != '\0') {
        *vga = *print;
        print++;
        vga += 2;
    }
    line++;
}

// Checks if CPUID is supported by attempting to flip the ID bit (bit 21) in EFLAGS.
// Returns 1 if CPUID is supported, 0 otherwise.
uint32_t check_cpuid(void) {
    uint32_t result;
    __asm__ volatile (
        "pushf\n\t"              // Save EFLAGS to stack
        "pop %%eax\n\t"           // Pop EFLAGS into eax
        "mov %%eax, %%ecx\n\t"    // Save original EFLAGS in ecx
        "xor $0x200000, %%eax\n\t"// Flip ID bit (1 << 21)
        "push %%eax\n\t"          // Push modified EFLAGS
        "popf\n\t"               // Load modified EFLAGS
        "pushf\n\t"              // Push EFLAGS again
        "pop %%eax\n\t"           // Pop into eax
        "push %%ecx\n\t"          // Restore original EFLAGS
        "popf\n\t"               // Load original EFLAGS
        "xor %%ecx, %%eax\n\t"    // Compare flipped vs original
        "jnz 1f\n\t"              // If different, CPUID supported
        "mov $0, %0\n\t"          // No CPUID, set result = 0
        "jmp 2f\n\t"
        "1:\n\t"
        "mov $1, %0\n\t"          // CPUID supported, set result = 1
        "2:\n\t"
        : "=r" (result)           // Output: result
        :                         // No inputs
        : "%eax", "%ecx", "cc"    // Clobbered: eax, ecx, condition codes
    );
    return result;
}
