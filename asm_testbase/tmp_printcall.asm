section .data
    message db 'Hello', 0  ; Null-terminated string to print

section .text
    extern print   ; Declare the external print function
    global _start  ; Declare the entry point for the program

_start:
    ; Load the address of the message into rsi (argument for print)
    lea rsi, [message]

    ; Call the print function
    call print

    ; Exit the program (using syscall)
    mov rax, 60        ; Syscall number for exit (60)
    xor rdi, rdi       ; Return code 0
    syscall            ; Make the syscall
