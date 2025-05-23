section .text

GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_getTime
GLOBAL sys_getRegisters
GLOBAL sys_clearScreen

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
    
    mov rax, 2          ; syscall number for getRegisters
    mov rdi, [rbp + 16] ; buffer address
    
    int 0x80
    
    mov rsp, rbp
    pop rbp
    ret

sys_clearScreen:
    push rbp
    mov rbp, rsp

    mov rax, 4          ;syscall number for clearScreen
    int 0x80

    mov rsp, rbp
    pop rbp 
    getRegisters

