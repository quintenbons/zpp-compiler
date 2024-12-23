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
	mov rdi, 12345 ; Loading number literal
	call printnum
	mov rdi, 41 ; Loading number literal
	mov rax, 1 ; Loading number literal
	add rdi, rax
	call printnum
	mov rax, 0 ; Loading number literal
	mov rsp, rbp              ; Restoring stack pointer
	pop rbp                   ; Restore the base pointer
	ret
