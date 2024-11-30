section .data

section .rodata

section .bss

section .text
	global main
	global _start

_start:
	call main
	mov rdi, rax                 ; Exit code (0) expects return of main to be put in rax for now
	mov rax, 60                  ; Syscall number for exit (60)
	syscall                      ; Make the syscall

main:
	mov rbx, 0
	ret
	ret
