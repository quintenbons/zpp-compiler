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
	mov rax, 1 ; Loading number literal
	test rax,rax
	jz ._U0_else
	mov rdi, 0 ; Loading number literal
	call printnum
	jmp ._U1_end_if
._U0_else:
	mov rdi, 1 ; Loading number literal
	call printnum
._U1_end_if:
	mov rax, 0 ; Loading number literal
	test rax,rax
	jz ._U2_else
	mov rdi, 2 ; Loading number literal
	call printnum
	jmp ._U3_end_if
._U2_else:
	mov rax, 1 ; Loading number literal
	test rax,rax
	jz ._U4_else
	mov rdi, 3 ; Loading number literal
	call printnum
	jmp ._U5_end_if
._U4_else:
	mov rdi, 4 ; Loading number literal
	call printnum
._U5_end_if:
._U3_end_if:
	mov rax, 1 ; Loading number literal
	test rax,rax
	jz ._U7_end_if
	mov rdi, 5 ; Loading number literal
	call printnum
._U7_end_if:
	mov rdi, 6 ; Loading number literal
	call printnum
	mov rax, 0 ; Loading number literal
	test rax,rax
	jz ._U8_else
	mov rdi, 7 ; Loading number literal
	call printnum
	jmp ._U9_end_if
._U8_else:
	mov rdi, 8 ; Loading number literal
	call printnum
._U9_end_if:
	mov rax, 0 ; Loading number literal
	mov rsp, rbp              ; Restoring stack pointer
	pop rbp                   ; Restore the base pointer
	ret
