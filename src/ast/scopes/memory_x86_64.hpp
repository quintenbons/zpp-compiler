#pragma once

#include <cstddef>
#include <variant>
#include <string>

#include "registers.hpp"

namespace scopes
{

struct GlobalStackOffset
{
  bitSize_t _size;
  byteSize_t _byteOffset;

  inline friend std::ostream& operator<<(std::ostream& os, const GlobalStackOffset& gso) {
    return os << "GlobalStackOffset: size=" << gso._size << ", byteOffset=" << gso._byteOffset;
  }
};

struct LocalStackOffset
{
  bitSize_t _size;
  byteSize_t _byteOffset;

  inline friend std::ostream& operator<<(std::ostream& os, const LocalStackOffset& lso) {
    return os << "LocalStackOffset: size=" << lso._size << ", byteOffset=" << lso._byteOffset;
  }
};

using LocationDescription = std::variant<
  Register,
  LocalStackOffset,
  GlobalStackOffset
>;

struct VariableDescription
{
  variableId_t variableId;
  std::string_view name;
  LocationDescription location;
};

}
