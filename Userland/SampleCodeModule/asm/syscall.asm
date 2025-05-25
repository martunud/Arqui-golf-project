section .text

GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_getTime
GLOBAL sys_getRegisters
GLOBAL sys_clearScreen
GLOBAL sys_beep
GLOBAL sys_sleep
GLOBAL sys_setFontScale

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
    
    int 0x80
    
    mov rsp, rbp
    pop rbp
    ret

sys_getRegisters:
    push rbp
    mov rbp, rsp
    
    mov rax, 3          ; syscall number for getRegisters
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
    ret

sys_beep:
    push rbp
    mov rbp, rsp
    mov rax, 5
    int 0x80
    mov rsp, rbp
    pop rbp
    ret

sys_sleep:
    push rbp
    mov rbp, rsp
    mov rax, 6
    int 0x80
    mov rsp, rbp
    pop rbp
    ret

sys_setFontScale:
    push rbp
    mov rbp, rsp
    mov rax, 7
    int 0x80
    mov rsp, rbp
    pop rbp
    ret