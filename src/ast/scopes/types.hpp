#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace scopes
{

using typeId_t = uint32_t;

struct TypeDescription
{
  typeId_t id;
  uint32_t size;
  std::string name;
};

enum PrimitiveTypeIndex
{
  IDX_VOID_TYPE,
  IDX_INT_TYPE,
};

const static TypeDescription VOID_TYPE = TypeDescription{ IDX_VOID_TYPE, 0, "void" };
const static TypeDescription INT_TYPE = TypeDescription{ IDX_INT_TYPE, 32, "int" };

inline std::vector<TypeDescription> generatePrimitiveTypeVector()
{
  return {
    VOID_TYPE,
    INT_TYPE,
  };
}

}
