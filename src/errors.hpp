#define THROW(msg)                           \
    do {                                     \
        std::cerr << msg << std::endl;       \
        std::exit(1);                        \
    } while (0)

#define DEBUG_ASSERT(cond, ...)                                                   \
    do {                                                                          \
        if (!(cond)) {                                                            \
            std::cerr << "Assertion failed: (" << #cond << "), file " << __FILE__ \
                      << ", line " << __LINE__;                                   \
            if constexpr (sizeof(#__VA_ARGS__) > 1) {                             \
                std::cerr << ": " << __VA_ARGS__;                                 \
            }                                                                     \
            std::cerr << std::endl;                                               \
            std::exit(1);                                                         \
        }                                                                         \
    } while (0)

#define USER_ASSERT(cond, msg, ...)                                                \
    do {                                                                           \
        if (!(cond)) {                                                             \
            __VA_OPT__(                                                            \
                const FilePosition& pos = __VA_ARGS__;                             \
                std::cerr << "[Line: " << pos.lineCount                            \
                          << ", Offset: " << pos.lineOffset << "] ";               \
            )                                                                      \
            std::cerr << "Build failed: " << msg;                                  \
            std::cerr << std::endl;                                                \
            std::exit(1);                                                          \
        }                                                                          \
    } while (0)
