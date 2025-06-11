global save_registers_snapshot

section .text
save_registers_snapshot:
    mov [rdi + 0*8], rax
    mov [rdi + 1*8], rbx
    mov [rdi + 2*8], rcx
    mov [rdi + 3*8], rdx
    mov [rdi + 4*8], rsi
    mov [rdi + 5*8], rdi
    mov [rdi + 6*8], rbp
    mov [rdi + 7*8], r8
    mov [rdi + 8*8], r9
    mov [rdi + 9*8], r10
    mov [rdi + 10*8], r11
    mov [rdi + 11*8], r12
    mov [rdi + 12*8], r13
    mov [rdi + 13*8], r14
    mov [rdi + 14*8], r15
    lea rax, [rel .return_address]
    mov [rdi + 15*8], rax      ; RIP
    mov rax, rsp
    mov [rdi + 16*8], rax      ; RSP
    pushfq
    pop rax
    mov [rdi + 17*8], rax      ; RFLAGS
.return_address:
    ret