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
    mov [registerdata], rax
    mov [registerdata + 1*8], rbx
    mov [registerdata + 2*8], rcx
    mov [registerdata + 3*8], rdx
    mov [registerdata + 4*8], rsi
    mov [registerdata + 5*8], rdi
    mov [registerdata + 6*8], rbp
    mov [registerdata + 7*8], r8
    mov [registerdata + 8*8], r9
    mov [registerdata + 9*8], r10
    mov [registerdata + 10*8], r11
    mov [registerdata + 11*8], r12
    mov [registerdata + 12*8], r13
    mov [registerdata + 13*8], r14
    mov [registerdata + 14*8], r15
    mov rax, [rsp]
    mov [registerdata + 15*8], rax ; instruction pointer
    mov rax, rsp
    mov [registerdata + 16*8], rax
    ret

_getRegisters:
    mov rax, registerdata
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
registerdata resq 17