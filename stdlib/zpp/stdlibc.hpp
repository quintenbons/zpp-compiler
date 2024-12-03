// This would be a good start:
// void* malloc(std::size_t size); // TODO inttypes
// void free(void* ptr);
// int printf(const char* format, ...);
// int scanf(const char* format, ...);
// [[noreturn]] void exit(int status);
// void* memcpy(void* dest, const void* src, std::size_t n);
// std::size_t strlen(const char* str);
// char* strcpy(char* dest, const char* src);
// void qsort(void* base, std::size_t nitems, std::size_t size, int (*compare)(const void*, const void*));
// int rand();
// void srand(unsigned int seed);
// size_t write(int fd, const void *buf, size_t count);

int printf(const char *format, ...);
std::size_t strlen(const char* str);
int print(const char *message); // not in the C nor C++ standard, just useful for us...
ssize_t write(int fd, const void *buf, size_t count);
[[noreturn]] void exit(int status);
void *malloc(size_t size);
void free(void *ptr);





