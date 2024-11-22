section .data
    msg db "Hello, World!", 0xA  ; Define the message (newline at the end)
    msg_len equ $ - msg          ; Calculate the length of the message

section .text
    global _start                ; Entry point for the program

_start:
    ; Write syscall (sys_write)
    mov rax, 1                   ; Syscall number for write (1)
    mov rdi, 1                   ; File descriptor (stdout is 1)
    mov rsi, msg                 ; Address of the message
    mov rdx, msg_len             ; Length of the message
    syscall                      ; Make the syscall

    ; Exit syscall (sys_exit)
    mov rax, 60                  ; Syscall number for exit (60)
    xor rdi, rdi                 ; Exit code (0)
    syscall                      ; Make the syscall

