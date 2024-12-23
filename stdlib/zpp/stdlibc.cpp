// #include <stdlibc.hpp> // TODO preprocessing step

// int write(int fd, const void *buf, int count)
// {
//   asm(
//     ""
//   );
// }

// void strlen(const char *) {}

void print(char *str) // not in the C nor C++ standard, just useful for us...
{
  // later will be something like this
  // return write(0, message, strlen(str));

  asm(
    "\tmov rax, 1\t\t\t; syscall number for write\n"
    "\tmov rdi, 1\t\t\t; fd = 1 (stdout)\n"
    "\t; mov rsi, ?\t\t\t; put the string pointer into rsi (already done)\n"
    "\tmov rdx, 5\n" // the size of the str is hardcoded
    "\tsyscall\n"
    : "=rsi"(str) // ask the compiler to put the char* in the rsi register before doing this code block
  );
}

void printnum(int num) {
  asm(
    // nasm elf64 asm here
    "\tmov rax, rdi            ; currently, rdi is the first argument register\n"
    "\tmov rsi, rsp            ; Use stack as buffer\n"
    "\tsub rsi, 32             ; Move buffer to a safe place\n"
    "\tmov byte [rsi], 0       ; Null-terminate\n"
    "\tmov rbx, 10             ; Divisor\n"
    "\tmov rcx, rsi            ; RCX will point to the current position in the buffer\n"
    "\t; Convert number to string in reverse\n"
".reverse_loop:\n"
    "\txor rdx, rdx            ; Clear rdx for division\n"
    "\tdiv rbx                 ; RAX / 10, result in RAX, remainder in RDX\n"
    "\tadd dl, '0'             ; Convert remainder to ASCII\n"
    "\tdec rcx                 ; Move back in the buffer\n"
    "\tmov [rcx], dl           ; Store the character\n"
    "\ttest rax, rax           ; Check if quotient is zero\n"
    "\tjnz .reverse_loop        ; Continue if not zero\n"
    "\tdec rcx                 ; Move back in the buffer\n"
    "\tmov byte [rcx], 0x0A     ; newline\n"
    "\t; Prepare to call write\n"
    "\tmov rdx, rsi            ; End of string position\n"
    "\tsub rdx, rcx            ; Calculate length of the string\n"
    "\tmov rsi, rcx            ; String start position\n"
    "\tmov rdi, 1              ; File descriptor 1 (stdout)\n"
    "\tmov eax, 1              ; Syscall number (sys_write)\n"
    "\tsyscall                 ; Perform syscall\n"
    : "=rax"(num)
  );
}
