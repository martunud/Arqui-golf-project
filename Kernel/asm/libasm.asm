GLOBAL cpuVendor
GLOBAL getScanCode
GLOBAL _refreshRegisters
GLOBAL _getRegisters
GLOBAL _readTime


section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret


getScanCode:
    xor rax, rax     ; Limpio el registro RAX
    in al, 0x60      ; Lee un byte del puerto del teclado (0x60) y lo guarda en AL (parte baja de RAX)
    ret              ; Devuelve el scancode en RAX 

_refreshRegisters:
    mov [_registerdata], rax
    mov [_registerdata + 1*8], rbx
    mov [_registerdata + 2*8], rcx
    mov [_registerdata + 3*8], rdx
    mov [_registerdata + 4*8], rsi
    mov [_registerdata + 5*8], rdi
    mov [_registerdata + 6*8], rbp
    mov [_registerdata + 7*8], r8
    mov [_registerdata + 8*8], r9
    mov [_registerdata + 9*8], r10
    mov [_registerdata + 10*8], r11
    mov [_registerdata + 11*8], r12
    mov [_registerdata + 12*8], r13
    mov [_registerdata + 13*8], r14
    mov [_registerdata + 14*8], r15
    mov rax, [rsp]
    mov [_registerdata + 15*8], rax ; instruction pointer
    mov rax, rsp
    mov [_registerdata + 16*8], rax
    ret

_getRegisters:
    mov rax, _registerdata
    ret

_readTime:
    push dx
    mov dx, 0x70
    mov al, dil
    out dx, al
    xor rax, rax
    in al, 0x71
    pop dx
    ret

section .bss
_registerdata: resq 17  ; Reserva espacio para 17 registros
