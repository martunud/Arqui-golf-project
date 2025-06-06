GLOBAL cpuVendor
GLOBAL getScanCode

GLOBAL _readTime
GLOBAL inb
GLOBAL outb

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


_readTime:
    push rbp
    mov rbp, rsp

    push dx
    mov dx, 0x70
    mov al, dil
    out dx, al
    xor rax, rax
    in al, 0x71
    pop dx

    mov rsp, rbp
    pop rbp
    ret
    ret

inb:
    push rbp
    mov rbp, rsp

    in al, dx

    mov rsp, rbp
    pop rbp
    ret

outb:
    push rbp
    mov rbp, rsp

    mov dx, di
    mov al, sil
    out dx, al    

    mov rsp, rbp
    pop rbp
    ret
