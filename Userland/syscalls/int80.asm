section .text
GLOBAL _syscall

_syscall:
	push rbp
	mov rbp, rsp

	push rbx
	push rsp
	push rbp
	push r12
	push r13
	push r14
	push r15
	
        ;reordenamos los parametros para pasarlos
        
	mov rax, rdi ;ponemos el primer parametro que es el ID de la syscall en rax
	mov rdi, rsi ;a partir de aca movemos los params hacia la izquierda
	mov rsi, rdx
	mov rdx, rcx
	mov rcx, r8

        int 0x80

	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	pop rsp
	pop rbx

	mov rsp, rbp
	pop rbp
	ret
