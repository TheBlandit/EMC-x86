BITS 16 ; Produce 16 bit code
ORG 0x7c00 ; Set the origin to 0x7c00 which is where BIOS loads the bootloader

_start:
    jmp 0:.ClearCS ; Set CS to 0 (some bios load at 07C0:0000 instead of 0000:7C000)

.ClearCS:
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
.loop:
    mov byte al, [si]

    cmp al, 0
    je read_kernel

    mov byte [es:di], al
    inc si
    add di, 2

    jmp .loop

error:
    mov ax, 0xb800          ; Set AX to the video memory segment (0xb800)
    mov es, ax              ; Load ES with the video memory segment
    xor di, di
    lea si, [error_message]
.loop:
    mov byte al, [si]

    cmp al, 0
    je halt

    mov byte [es:di], al
    inc si
    add di, 2

    jmp .loop

halt:
    cli
.loop:
    hlt
    jmp .loop

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
    jmp 08h:jump_32 ; far jump to callc

BITS 32
jump_32:
    mov ax, 10h
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x10000

    call 1000h ; _start in C

    ; Enable PAE
    mov eax, cr4
    or eax, 0b100000
    mov cr4, eax

    ; Set CR3
    mov eax, 1 << 20
    mov cr3, eax

    ; Set IA32_EFER.LME
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x100
    wrmsr

    ; Enable Paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    jmp 8h:jump_64

BITS 64
jump_64:
    mov ax, 10h
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov rsp, 0x10000

    call 2000h ; _start in C
    jmp halt

message db 'Hello world', 0 ; The 'Hello World' message followed by a null terminator (0)
error_message db 'Failed to read kernel from disk', 0
gdt_location equ 0x0800 ; gdt location
gdt_contents:
    dq 0x000000000800001F
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x0000000000000000
gdt_size equ $ - gdt_contents ; gdt location
sectors_read equ 0x10
; Boot sector padding and signature
times 510-($-$$) db 0   ; Pad the boot sector to 510 bytes (ensuring the total size is 512 bytes)
dw 0xAA55               ; Boot sector signature (0xAA55), required for a valid bootable sector
