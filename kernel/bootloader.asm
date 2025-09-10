BITS 16                     ; Instruct NASM that this is 16 bit (real mode) code
org 0x7c00                  ; Set the origin to 0x7c00 which is where BIOS loads the bootloader

_start:
    mov ax, 0xb800          ; Set AX to the video memory segment (0xb800)
    mov es, ax              ; Load ES with the video memory segment

    xor di, di              ; Start at the beginning of the video memory (offset 0)
    mov cx, 2000            ; 80 characters wide and 25 characters tall

clearscreen:
    mov byte [es:di], 0x20  ; Write character (space character) to video memory at ES:DI
    inc di                  ; Move to next byte (the attribute)

    mov byte [es:di], 0x07  ; Write attribute (light gray on black) to video memory at ES:DI
    inc di                  ; Move to the next byte (the character)

    dec cx                  ; Decrement CX
    jnz clearscreen         ; Jump to 'clearscreen' if CX is not zero

    lea si, [message]       ; Load the effective address of 'message' into SI
    xor di, di              ; Start at the beginning of the video memory (offset 0)

init_line:
    mov byte al, [si]

    cmp al, 0
    je init_read

    mov byte [es:di], al
    inc si
    add di, 2

    jmp init_line

init_read:
    mov word di, 0x0F00

read_loop:
    call read_char

    cmp ah, 0x1C
    je parse_init

    cmp ah, 0x0E
    je backspace

    cmp ah, 0x01
    je halt

    cmp di, 0x0F9E
    je read_loop

    mov byte [es:di], al
    add di, 2
    jmp read_loop

backspace:
    cmp di, 0x0F00
    je read_loop

    sub di, 2
    mov byte [es:di], 0x20
    jmp read_loop

parse_error:
    jmp halt ; TODO:

; Loops through chars until it finds a non-numerical where it breaks and starts the collatz loop
parse_init: ; TODO: check for overflow
    mov word si, 0x0F00
    xor ax, ax ; Set number t 0
    xor bh, bh ; Clears so when we add the number read to ax, there is no unexpected bits
    mov cx, 0x0A ; Puts 10 into cx for multiplication

parse_loop:
    mov byte bl, [es:si]

    cmp bl, 0x30
    jb collatz_init

    cmp bl, 0x39
    ja collatz_init

    sub bl, 0x30

    mul cx
    add ax, bx

    add si, 2

    jmp parse_loop

; ax contains number
collatz_init:
    call push_lines_init
    mov bx, 3

collatz_loop:
    push ax
    call print_num_init
    call sleep_sec
    call push_lines_init
    pop ax

    cmp ax, 1
    jbe collatz_finished

    test ax, 1
    jz collatz_even

    mul bx
    inc ax

    jmp collatz_loop

collatz_even:
    shr ax, 1
    jmp collatz_loop

collatz_finished:

    jmp init_read

; SUBROUTINES ---------------

halt:
    cli
    hlt

; input in ax
; overwrites di, dx, cx, ax
print_num_init:
    mov word di, 0x0FA0
    mov word cx, 0x0A

print_num_loop:
    xor dx, dx
    div cx

    sub di, 2

    add dx, 0x30
    mov byte [es:di], dl

    cmp ax, 0
    jne print_num_loop

    ret

; Overwrites cx, dx, ah
sleep_sec:
    mov dx, 0x4240
    mov cx, 0x000F
    mov ah, 0x86
    int 0x15

    ret
; Overwrites: di, si, dx
; di, si are in the first column in the row below the screen
; Pushes all lines up 1 and sets the bottom line to empty, does not affect status bar
push_lines_init:
    mov di, 0x00A0
    mov si, 0x0140

push_lines_loop1: ; push lines up
    mov word dx, [es:si]
    mov word [es:di], dx
    add di, 2
    add si, 2

    cmp si, 0x0FA0
    jb push_lines_loop1

push_lines_loop2: ; sets current line to spaces
    mov byte [es:di], 0x20
    add di, 2

    cmp di, 0x0FA0
    jb push_lines_loop2

    ret

; overwrites ax, al contains the ascii code, ah contains the scan code 
read_char:
    mov ah, 0x00 ; BIOS fn to read char
    int 0x16 ; Calls BIOS for key interrupt
    ret

setup_gdt:
    mov di, gdt_location
    


enable_protected:
    cli ; disable interrupts
    lgdt gdt_location ; load the gdtr with the location of the gdt
    mov eax, cr0 ; load cr0 into eax
    or eax, 1 ; set PE
    mov cr0, eax ; load eax back into cr0 (with PE enabled)

    jmp 01h:1000h ; _start in C (far jump since cs changes)


message db 'Hello world', 0 ; The 'Hello World' message followed by a null terminator (0)
; message_len equ $ - message
gdt_location equ 0x2000 ; gdt location
gdt_size equ 0x2000 ; gdt location

; Boot sector padding and signature
    times 510-($-$$) db 0   ; Pad the boot sector to 510 bytes (ensuring the total size is 512 bytes)
    dw 0xAA55               ; Boot sector signature (0xAA55), required for a valid bootable sector
