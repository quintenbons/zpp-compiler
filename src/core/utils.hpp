#include <charconv>
#include <string_view>

namespace utils
{

template<typename T>
T readNumber(std::string_view str) {
    T value = 0;
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

    if (ec == std::errc::invalid_argument) {
        std::cerr << "Invalid argument: unable to convert to number" << std::endl;
    } else if (ec == std::errc::result_out_of_range) {
        std::cerr << "Out of range: number is too large" << std::endl;
    }

    return value;
}

}
