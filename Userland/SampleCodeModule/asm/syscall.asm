section .text

GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_getTime
GLOBAL sys_getRegisters

sys_read:
    push rbp
    mov rbp, rsp
    mov rax, 0
    int 0x80
    mov rsp, rbp
    pop rbp
    ret

sys_write:
    push rbp
    mov rbp, rsp
    mov rax, 1
    int 0x80
    mov rsp, rbp
    pop rbp
    ret

sys_getTime:
    push rbp
    mov rbp, rsp
    
    mov rax, 2          ; syscall number for getTime
    mov rdi, [rbp + 16] ; buffer address
    
    int 0x80
    
    mov rsp, rbp
    pop rbp
    ret

sys_getRegisters:
    push rbp
    mov rbp, rsp
    
    mov rax, 3          ; syscall number for getRegisters
    mov rdi, [rbp + 16] ; buffer address
    xor rsi, rsi
    xor rdx, rdx
    xor r10, r10
    xor r8, r8
    
    int 0x80
    
    leave
    ret