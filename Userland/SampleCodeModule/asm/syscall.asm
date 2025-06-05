section .text

GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_getTime
GLOBAL sys_getRegisters
GLOBAL sys_clearScreen
GLOBAL sys_beep
GLOBAL sys_sleep
GLOBAL sys_setFontScale
GLOBAL sys_takeRegistersSnapshot

GLOBAL sys_video_putPixel
GLOBAL sys_video_putChar
GLOBAL sys_video_clearScreenColor
global sys_video_putCharXY



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

sys_takeRegistersSnapshot:
    push rbp
    mov rbp, rsp
    mov rax, 8
    mov rdi, rdi      ; Primer argumento: puntero al buffer de registros
    int 0x80
    mov rsp, rbp
    pop rbp
    ret

section .text

sys_video_clearScreenColor:
    push rbp
    mov rbp, rsp
    mov rax, 9         ; Usar syscall 9 (clearScreenColor existente)
    ; rdi ya contiene el color
    int 0x80
    mov rsp, rbp
    pop rbp
    ret

sys_video_putPixel:
    push rbp
    mov rbp, rsp
    mov rax, 10         ; Número de syscall para putPixel
    ; Los parámetros ya están en rdi (x), rsi (y), rdx (color)
    int 0x80
    mov rsp, rbp
    pop rbp
    ret

sys_video_putChar:
    push rbp
    mov rbp, rsp
    mov rax, 11         ; Número de syscall para putChar
    ; Los parámetros ya están en rdi (char), rsi (fg), rdx (bg)
    int 0x80
    mov rsp, rbp
    pop rbp
    ret

sys_video_putCharXY:
    push rbp
    mov rbp, rsp
    mov rax, 12
    ; rdi = c, rsi = x, rdx = y, rcx = fg, r8 = bg
    int 0x80
    mov rsp, rbp
    pop rbp
    ret