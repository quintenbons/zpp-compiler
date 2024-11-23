// #include <stdlibc.hpp> // TODO preprocessing step

// int write(int fd, const void *buf, int count)
// {
//   asm(
//     ""
//   );
// }

// void strlen(const char *) {}

void print(const char *str) // not in the C nor C++ standard, just useful for us...
{
  // later will be something like this
  // return write(0, message, strlen(str));

  asm(
    "\tmov rax, 1\t\t\t; syscall number for write\n"
    "\tmov rdi, 1\t\t\t; fd = 1 (stdout)\n"
    "\t; mov rsi, ?\t\t\t; put the string pointer into rsi (already done)\n"
    "\tmov rdx, 5\n" // the size of the str is hardcoded
    "\tsyscall\n"
    : "rsi"(str) // ask the compiler to put the char* in the rsi register before doing this code block
  );
}
