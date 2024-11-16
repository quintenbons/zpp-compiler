#define THROW(msg)                           \
    do {                                     \
        std::cerr << msg << std::endl;       \
        std::exit(1);                        \
    } while (0)

#define ASSERT(cond, ...)                                                        \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::cerr << "Assertion failed: (" << #cond << "), file " << __FILE__ \
                      << ", line " << __LINE__;                                  \
            if constexpr (sizeof(#__VA_ARGS__) > 1) {                            \
                std::cerr << ": " << __VA_ARGS__;                                \
            }                                                                    \
            std::cerr << std::endl;                                              \
            std::exit(1);                                                        \
        }                                                                        \
    } while (0)
