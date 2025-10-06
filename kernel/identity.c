// Assumes 256MiB (28 bits) physical memory can be paged
// First MiB is not written to since there is often BIOS/UEFI data in it
// Uses level 4 paging

#include <stdint.h>
#include "identity.h"

const uint64_t FLAGS = 0b000000000011; // R/W, P
const uintptr_t CR3 = 1 << 20;

void create_pde(uint64_t* base) {
    uint64_t* pte_base = base + 512;
    for (uintptr_t i = 0; i < 128; i++) {
        *base = (uintptr_t)pte_base | FLAGS;
        base++;
        pte_base += 512;
    }

    // Set remaining entries to 0
    for (uintptr_t i = 0; i < 384; i++) {
        *base = 0;
        base++;
    }

    // Creates 256 * 512 Pages
    uint64_t dst = 0;
    for (uintptr_t i = 0; i < 512 * 128; i++) {
        *base = dst | 3;
        base++;
        dst += 1 << 12;
    }
}

/// Only allocate first PDPTE and return its base address
/// base must be aligned to 4KiB and in a low address (eg 1MiB)
/// (also used to create pdpte since it has the same structure)
uint64_t* create_plm4e(uint64_t* base) {
    // Zero all other entries (P flag is 0)
    for (uintptr_t i = 1; i < 512; i++) {
        base[i] = 0;
    }

    uint64_t* pdpte = base + 512;
    *base = (uint64_t)pdpte | FLAGS;
    return pdpte;
}

void paging_create() {
    uint64_t* pdpte = create_plm4e((uint64_t*)CR3);
    uint64_t* pde = create_plm4e(pdpte);
    create_pde(pde);
}
