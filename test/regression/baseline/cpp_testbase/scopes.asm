section .data

section .rodata

section .bss

section .text

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
	sub rsp, 4 ; Creating space on the stack
	sub rsp, 4 ; Creating space on the stack
	mov rsp, rbp              ; Restoring stack pointer
	pop rbp                   ; Restore the base pointer
	ret
