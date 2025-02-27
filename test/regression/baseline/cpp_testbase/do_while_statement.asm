section .data

section .rodata

section .bss

section .text
	extern printnum:function

	global main:function
	global _start:function

_start:
	call main
	mov rdi, rax            ; Exit code (0) expects return of main to be put in rax for now
	mov rax, 60                  ; Syscall number for exit (60)
	syscall                      ; Make the syscall

main:
	push rbp                 ; Save the base pointer
	mov rbp, rsp              ; Set base pointer to current stack pointer
	sub rsp, 4 ; Creating space on the stack
	mov rax, 5 ; Loading number literal
	mov [rbp-4], eax ; Storing value in memory
._U0_do-while:
	mov edi, [rbp-4] ; Loading value from memory
	call printnum
	mov eax, [rbp-4] ; Loading value from memory
	mov rbx, 1 ; Loading number literal
	sub rax, rbx
	mov [rbp-4], rax ; Storing value in memory
	mov eax, [rbp-4] ; Loading value from memory
	test rax,rax
	jnz._U0_do-while
	mov rax, 0 ; Loading number literal
	mov rsp, rbp              ; Restoring stack pointer
	pop rbp                   ; Restore the base pointer
	ret
