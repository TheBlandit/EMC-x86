extern function_from_file2

section .text
    global _start

_start:
    ; Call a function from file2
    call function_from_file2
    ; Exit the program
    mov eax, 60      ; syscall: exit
    xor edi, edi     ; status: 0
    syscall

