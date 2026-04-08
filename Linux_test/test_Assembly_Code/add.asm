section .data
    num1    dq 5              ; 64-bit value (quadword)
    num2    dq 7

section .bss
    result  resq 1            ; reserve 64-bit space

section .text
    global _start

_start:
    ; Load values from memory
    mov rax, [num1]           ; rax = 5
    mov rbx, [num2]           ; rbx = 7

    ; Add them
    add rax, rbx              ; rax = 5 + 7 = 12

    ; Store result (optional)
    mov [result], rax

    ; Exit program, return rax as exit code
    ; Note: Linux only uses the low 8 bits (0-255)
    mov rdi, rax              ; rdi = exit code
    mov rax, 60               ; syscall number for exit
    syscall

