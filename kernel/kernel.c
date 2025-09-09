void _start() {
    while (1) {
        asm volatile("hlt");
    }
}

const char volatile* VGA_BUFFER = (char volatile*)0xb8000;

