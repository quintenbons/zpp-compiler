section .data

section .rodata

section .bss

section .text
	global print:function

print:
;-- START -- asm binding requests
	mov rsi, rax
;-- START -- user defined
	mov rax, 1			; syscall number for write
	mov rdi, 1			; fd = 1 (stdout)
	; mov rsi, ?			; put the string pointer into rsi (already done)
	mov rdx, 5
	syscall

;-- END -- user defined
	ret
