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

#define FUNDAMENTAL_TYPE_LIST \
  X(VOID)                     \
  X(CHAR)                     \
  X(INT)                       

enum PrimitiveTypeIndex
{
#define X(type) IDX_##type##_TYPE,
  FUNDAMENTAL_TYPE_LIST  
#undef X
};


const static TypeDescription VOID_TYPE = TypeDescription{ IDX_VOID_TYPE, 0, "void" };
const static TypeDescription CHAR_TYPE = TypeDescription{ IDX_CHAR_TYPE, 1, "char" };
const static TypeDescription INT_TYPE = TypeDescription{ IDX_INT_TYPE, 4, "int" };

const static byteSize_t ADDRESS_SIZE = 8;

inline std::vector<TypeDescription> generatePrimitiveTypeVector()
{
  return {
    VOID_TYPE,
    CHAR_TYPE,
    INT_TYPE,
  };
}

}
