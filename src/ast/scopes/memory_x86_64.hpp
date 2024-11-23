#pragma once

#include <cstddef>
#include <variant>
#include <string>

#include "registers.hpp"

namespace scopes
{

struct GlobalStackOffset
{
  size_t _size;
  size_t _byteOffset;
};

struct LocalStackOffset
{
  size_t _size;
  size_t _byteOffset;
};

using LocationDescription = std::variant<
  Register,
  LocalStackOffset,
  GlobalStackOffset
>;

struct VariableDescription
{
  std::string name;
  LocationDescription location;
};

}
