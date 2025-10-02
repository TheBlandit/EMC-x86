bits 16                     ; Instruct NASM that this is 16 bit (real mode) code
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
    je read_kernel

    mov byte [es:di], al
    inc si
    add di, 2

    jmp init_line

error:
    mov ax, 0xb800          ; Set AX to the video memory segment (0xb800)
    mov es, ax              ; Load ES with the video memory segment
    xor di, di
    lea si, [error_message]

error_loop:
    mov byte al, [si]

    cmp al, 0
    je halt

    mov byte [es:di], al
    inc si
    add di, 2

    jmp error_loop

halt:
    cli
    hlt

read_kernel:
    xor ax, ax      ; Clear AX
    mov ds, ax      ; Set DS to 0 (flat real-mode addressing)
    mov es, ax      ; Set ES to 0 (destination segment for read)

    mov ah, 0x02    ; BIOS function: read sectors
    mov al, sectors_read ; Number of sectors to read
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Sector 2 (1-based indexing)
    mov dh, 0       ; Head 0
    mov dl, 0x80    ; Boot disk (0x80 = first hard disk, 0x00 = floppy)
    mov bx, 0x1000  ; Destination address ES:BX = 0x0:0x1000
    int 0x13        ; Call BIOS disk service

    jc error        ; If carry flag set, handle error
    cmp al, sectors_read ; Check if all sector were read
    jne error       ; If not, handle error

    ; setup gdt
    lea di, [gdt_location]
    lea si, [gdt_contents]
    ; first entry is the lgdt operand / null segment descriptor
    mov cx, gdt_size

setup_gdt_loop:
    jcxz enable_protection
    mov byte al, [si]
    mov byte [di], al
    inc si
    inc di
    dec cx
    jmp setup_gdt_loop

enable_protection:
    cli ; disable interrupts

    ; enable A20 (doesnt appear to be required)
    in al, 0x92
    or al, 2
    out 0x92, al

    ; in al, 0x92
    ; test al, 2
    ; jnz after
    ; or al, 2
    ; and al, 0xFE
    ; out 0x92, al
    ; after:

    lgdt [gdt_location] ; load the gdtr with the location of the gdt

    mov eax, cr0 ; load cr0 into eax
    or al, 1 ; set PE
    mov cr0, eax ; load eax back into cr0 (with PE enabled)
    jmp 08h:callc ; far jump to callc

bits 32
callc:
    mov ax, 10h
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x10000

    jmp 1000h ; _start in C

message db 'Hello world', 0 ; The 'Hello World' message followed by a null terminator (0)
error_message db 'Failed to read kernel from disk', 0
gdt_location equ 0x0800 ; gdt location
;               limit-1  base     base  junk
gdt_contents db 0x1F, 0, 0, 0x08, 0, 0, 0, 0,     0xFF, 0xFF, 0, 0, 0, 0x9A, 0xCF, 00,     0xFF, 0xFF, 0, 0, 0, 0x92, 0xCF, 0,    0, 0, 0, 0, 0, 0, 0, 0
gdt_size equ $ - gdt_contents ; gdt location
sectors_read equ 0x10
; Boot sector padding and signature
times 510-($-$$) db 0   ; Pad the boot sector to 510 bytes (ensuring the total size is 512 bytes)
dw 0xAA55               ; Boot sector signature (0xAA55), required for a valid bootable sector
