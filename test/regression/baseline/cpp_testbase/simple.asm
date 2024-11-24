section .data

section .rodata

section .bss

section .text
	global _start

_start:
	call main
	mov rax, 60                  ; Syscall number for exit (60)
	mov rdi, rbx                 ; Exit code (0) expects return of main to be put in rbx for now
	syscall                      ; Make the syscall

main:
  mov rbx, 1
  ret
	ret
