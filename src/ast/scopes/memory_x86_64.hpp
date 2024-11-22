#include <cstddef>

namespace scopes
{

enum RegisterID
{
  eax,
  ebx,
  ecx,
};

struct MemoryLocation
{

};

struct GlobalStack
{
  size_t _size;
  size_t _byteOffset;
};

struct LocalStack
{
  size_t _size;
  size_t _byteOffset;
};

struct Register
{
  RegisterID id;
};

}
