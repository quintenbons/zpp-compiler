#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace scopes
{

using id_t = uint32_t;
using byteSize_t = uint32_t;

struct TypeDescription
{
  id_t id;
  byteSize_t byteSize;
  std::string name;
};

enum PrimitiveTypeIndex
{
  IDX_VOID_TYPE,
  IDX_CHAR_TYPE,
  IDX_INT_TYPE,
};

const static TypeDescription VOID_TYPE = TypeDescription{ IDX_VOID_TYPE, 0, "void" };
const static TypeDescription CHAR_TYPE = TypeDescription{ IDX_INT_TYPE, 1, "char" };
const static TypeDescription INT_TYPE = TypeDescription{ IDX_INT_TYPE, 4, "int" };

inline std::vector<TypeDescription> generatePrimitiveTypeVector()
{
  return {
    VOID_TYPE,
    CHAR_TYPE,
    INT_TYPE,
  };
}

}
