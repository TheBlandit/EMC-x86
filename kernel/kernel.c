void _start() {
    char volatile* vga = (char volatile*)0xb8000;
    char* print = "Hello from 32-bit C running in protected mode";

    while (*print != '\0') {
        *vga = *print;
        print++;
        vga += 2;
    }

    while (1) {
        asm volatile("hlt");
    }
}
