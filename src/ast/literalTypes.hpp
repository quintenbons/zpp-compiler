#pragma once

#include <cstdint>
#include "scopes/types.hpp"

namespace ast
{
    using NumberLiteralUnderlyingType = uint64_t;
    constexpr scopes::byteSize_t NumberLiteralUnderlyingTypeSize = 8;
}