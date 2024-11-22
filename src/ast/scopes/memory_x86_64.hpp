#pragma once

#include <cstddef>
#include <variant>
#include <string>

namespace scopes
{

enum RegisterID
{
  eax,
  ebx,
  ecx,
};

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

struct Register
{
  RegisterID id;
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
