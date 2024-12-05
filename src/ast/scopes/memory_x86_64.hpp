#pragma once

#include <cstddef>
#include <variant>
#include <string>
#include <optional>

#include "registers.hpp"

namespace scopes
{

struct GlobalStackOffset
{
  byteSize_t _byteSize;
  byteSize_t _byteOffset;

  inline friend std::ostream& operator<<(std::ostream& os, const GlobalStackOffset& gso) {
    return os << "GlobalStackOffset: byteSize=" << gso._byteSize << ", byteOffset=" << gso._byteOffset;
  }
};

struct LocalStackOffset
{
  byteSize_t _byteSize;
  byteSize_t _byteOffset;

  inline friend std::ostream& operator<<(std::ostream& os, const LocalStackOffset& lso) {
    return os << "LocalStackOffset: byteSize=" << lso._byteSize << ", byteOffset=" << lso._byteOffset;
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
  std::optional<const TypeDescription*> typeDescription;
};

}
