#include <stdint.h>

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

    while (1) {
        __asm__ volatile("hlt");
    }
}

void println(char* print) {
    static uint32_t line = 0;
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
