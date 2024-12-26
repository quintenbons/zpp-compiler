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
	sub rsp, 4 ; Creating space on the stack
	mov rax, 4 ; Loading number literal
	mov [rbp-8], eax ; Storing value in memory
	sub rsp, 4 ; Creating space on the stack
	mov rax, 4 ; Loading number literal
	mov [rbp-12], eax ; Storing value in memory
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	sete al
	test rax,rax
	jz ._U1_end_if
	mov rdi, 1 ; Loading number literal
	call printnum
._U1_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	sete al
	test rax,rax
	jz ._U3_end_if
	mov rdi, 2 ; Loading number literal
	call printnum
._U3_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	sete al
	test rax,rax
	jz ._U5_end_if
	mov rdi, 3 ; Loading number literal
	call printnum
._U5_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setne al
	test rax,rax
	jz ._U7_end_if
	mov rdi, 4 ; Loading number literal
	call printnum
._U7_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setne al
	test rax,rax
	jz ._U9_end_if
	mov rdi, 5 ; Loading number literal
	call printnum
._U9_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setne al
	test rax,rax
	jz ._U11_end_if
	mov rdi, 6 ; Loading number literal
	call printnum
._U11_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setle al
	test rax,rax
	jz ._U13_end_if
	mov rdi, 7 ; Loading number literal
	call printnum
._U13_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setle al
	test rax,rax
	jz ._U15_end_if
	mov rdi, 8 ; Loading number literal
	call printnum
._U15_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setle al
	test rax,rax
	jz ._U17_end_if
	mov rdi, 9 ; Loading number literal
	call printnum
._U17_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setge al
	test rax,rax
	jz ._U19_end_if
	mov rdi, 10 ; Loading number literal
	call printnum
._U19_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setge al
	test rax,rax
	jz ._U21_end_if
	mov rdi, 11 ; Loading number literal
	call printnum
._U21_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setge al
	test rax,rax
	jz ._U23_end_if
	mov rdi, 12 ; Loading number literal
	call printnum
._U23_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setl al
	test rax,rax
	jz ._U25_end_if
	mov rdi, 13 ; Loading number literal
	call printnum
._U25_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setl al
	test rax,rax
	jz ._U27_end_if
	mov rdi, 14 ; Loading number literal
	call printnum
._U27_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setl al
	test rax,rax
	jz ._U29_end_if
	mov rdi, 15 ; Loading number literal
	call printnum
._U29_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setg al
	test rax,rax
	jz ._U31_end_if
	mov rdi, 16 ; Loading number literal
	call printnum
._U31_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setg al
	test rax,rax
	jz ._U33_end_if
	mov rdi, 17 ; Loading number literal
	call printnum
._U33_end_if:
	mov eax, [rbp-4] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setg al
	test rax,rax
	jz ._U35_end_if
	mov rdi, 18 ; Loading number literal
	call printnum
._U35_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	sete al
	test rax,rax
	jz ._U37_end_if
	mov rdi, 19 ; Loading number literal
	call printnum
._U37_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	sete al
	test rax,rax
	jz ._U39_end_if
	mov rdi, 20 ; Loading number literal
	call printnum
._U39_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	sete al
	test rax,rax
	jz ._U41_end_if
	mov rdi, 21 ; Loading number literal
	call printnum
._U41_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setne al
	test rax,rax
	jz ._U43_end_if
	mov rdi, 22 ; Loading number literal
	call printnum
._U43_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setne al
	test rax,rax
	jz ._U45_end_if
	mov rdi, 23 ; Loading number literal
	call printnum
._U45_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setne al
	test rax,rax
	jz ._U47_end_if
	mov rdi, 24 ; Loading number literal
	call printnum
._U47_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setle al
	test rax,rax
	jz ._U49_end_if
	mov rdi, 25 ; Loading number literal
	call printnum
._U49_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setle al
	test rax,rax
	jz ._U51_end_if
	mov rdi, 26 ; Loading number literal
	call printnum
._U51_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setle al
	test rax,rax
	jz ._U53_end_if
	mov rdi, 27 ; Loading number literal
	call printnum
._U53_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setge al
	test rax,rax
	jz ._U55_end_if
	mov rdi, 28 ; Loading number literal
	call printnum
._U55_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setge al
	test rax,rax
	jz ._U57_end_if
	mov rdi, 29 ; Loading number literal
	call printnum
._U57_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setge al
	test rax,rax
	jz ._U59_end_if
	mov rdi, 30 ; Loading number literal
	call printnum
._U59_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setl al
	test rax,rax
	jz ._U61_end_if
	mov rdi, 31 ; Loading number literal
	call printnum
._U61_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setl al
	test rax,rax
	jz ._U63_end_if
	mov rdi, 32 ; Loading number literal
	call printnum
._U63_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setl al
	test rax,rax
	jz ._U65_end_if
	mov rdi, 33 ; Loading number literal
	call printnum
._U65_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setg al
	test rax,rax
	jz ._U67_end_if
	mov rdi, 34 ; Loading number literal
	call printnum
._U67_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setg al
	test rax,rax
	jz ._U69_end_if
	mov rdi, 35 ; Loading number literal
	call printnum
._U69_end_if:
	mov eax, [rbp-8] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setg al
	test rax,rax
	jz ._U71_end_if
	mov rdi, 36 ; Loading number literal
	call printnum
._U71_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	sete al
	test rax,rax
	jz ._U73_end_if
	mov rdi, 37 ; Loading number literal
	call printnum
._U73_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	sete al
	test rax,rax
	jz ._U75_end_if
	mov rdi, 38 ; Loading number literal
	call printnum
._U75_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	sete al
	test rax,rax
	jz ._U77_end_if
	mov rdi, 39 ; Loading number literal
	call printnum
._U77_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setne al
	test rax,rax
	jz ._U79_end_if
	mov rdi, 40 ; Loading number literal
	call printnum
._U79_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setne al
	test rax,rax
	jz ._U81_end_if
	mov rdi, 41 ; Loading number literal
	call printnum
._U81_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setne al
	test rax,rax
	jz ._U83_end_if
	mov rdi, 42 ; Loading number literal
	call printnum
._U83_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setle al
	test rax,rax
	jz ._U85_end_if
	mov rdi, 43 ; Loading number literal
	call printnum
._U85_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setle al
	test rax,rax
	jz ._U87_end_if
	mov rdi, 44 ; Loading number literal
	call printnum
._U87_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setle al
	test rax,rax
	jz ._U89_end_if
	mov rdi, 45 ; Loading number literal
	call printnum
._U89_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setge al
	test rax,rax
	jz ._U91_end_if
	mov rdi, 46 ; Loading number literal
	call printnum
._U91_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setge al
	test rax,rax
	jz ._U93_end_if
	mov rdi, 47 ; Loading number literal
	call printnum
._U93_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setge al
	test rax,rax
	jz ._U95_end_if
	mov rdi, 48 ; Loading number literal
	call printnum
._U95_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setl al
	test rax,rax
	jz ._U97_end_if
	mov rdi, 49 ; Loading number literal
	call printnum
._U97_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setl al
	test rax,rax
	jz ._U99_end_if
	mov rdi, 50 ; Loading number literal
	call printnum
._U99_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setl al
	test rax,rax
	jz ._U101_end_if
	mov rdi, 51 ; Loading number literal
	call printnum
._U101_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-8] ; Loading value from memory
	cmp rax,rbx
	setg al
	test rax,rax
	jz ._U103_end_if
	mov rdi, 52 ; Loading number literal
	call printnum
._U103_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-12] ; Loading value from memory
	cmp rax,rbx
	setg al
	test rax,rax
	jz ._U105_end_if
	mov rdi, 53 ; Loading number literal
	call printnum
._U105_end_if:
	mov eax, [rbp-12] ; Loading value from memory
	mov ebx, [rbp-4] ; Loading value from memory
	cmp rax,rbx
	setg al
	test rax,rax
	jz ._U107_end_if
	mov rdi, 54 ; Loading number literal
	call printnum
._U107_end_if:
	mov rax, 0 ; Loading number literal
	mov rsp, rbp              ; Restoring stack pointer
	pop rbp                   ; Restore the base pointer
	ret
