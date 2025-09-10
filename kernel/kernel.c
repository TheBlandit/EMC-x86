void _start() {
    char volatile* vga = (char volatile*)0xb8000;
    *vga = 'a';
    while (1) {
        asm volatile("hlt");
    }
}
