#pragma once

#include <iostream>
#include <charconv>
#include <type_traits>
#include <variant>
#include <string_view>

namespace utils
{

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define INDENT_D(depth) std::string(depth * 2, ' ')

template<typename T> struct is_variant : std::false_type {};

template<typename ...Args>
struct is_variant<std::variant<Args...>> : std::true_type {};

template<typename T>
inline constexpr bool is_variant_v=is_variant<T>::value;

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
