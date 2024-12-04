#pragma once

#include <cstdint>
#include <string_view>
#include <unordered_map>

#include "dbg/errors.hpp"

namespace scopes
{

// Basically this file hardcodes this:
// Register | Size | Name
// ---------|------|------------------
// rax      | 64   | rax (general purpose. same with rbx, rcx, rdx)
//          | 32   | eax
//          | 16   | ax
//          | 8    | al/ah (low/high)
// rsi      | 64   | rsi (source for string ops)
//          | 32   | esi
//          | 16   | si
//          | 8    | sil
// rdi      | 64   | rdi (destination for string ops)
//          | 32   | edi
//          | 16   | di
//          | 8    | dil
// rbp      | 64   | rbp (stack base pointer)
//          | 32   | ebp
//          | 16   | bp
//          | 8    | bpl
// rsp      | 64   | rsp (stack pointer)
//          | 32   | esp
//          | 16   | sp
//          | 8    | spl
// r8       | 64   | r8 (up to r15)
//          | 32   | r8d
//          | 16   | r8w
//          | 8    | r8b

#define REGISTER_ID_64_LIST \
  X(REG_RAX, "rax") \
  X(REG_RBX, "rbx") \
  X(REG_RCX, "rcx") \
  X(REG_RDX, "rdx") \
  X(REG_RSI, "rsi") \
  X(REG_RDI, "rdi") \
  X(REG_RBP, "rbp") \
  X(REG_RSP, "rsp") \
  X(REG_R8, "r8") \
  X(REG_R9, "r9") \
  X(REG_R10, "r10") \
  X(REG_R11, "r11") \
  X(REG_R12, "r12") \
  X(REG_R13, "r13") \
  X(REG_R14, "r14") \
  X(REG_R15, "r15")

#define REGISTER_ID_32_LIST \
  X(REG_EAX, "eax") \
  X(REG_EBX, "ebx") \
  X(REG_ECX, "ecx") \
  X(REG_EDX, "edx") \
  X(REG_ESI, "esi") \
  X(REG_EDI, "edi") \
  X(REG_EBP, "ebp") \
  X(REG_ESP, "esp") \
  X(REG_R8D, "r8d") \
  X(REG_R9D, "r9d") \
  X(REG_R10D, "r10d") \
  X(REG_R11D, "r11d") \
  X(REG_R12D, "r12d") \
  X(REG_R13D, "r13d") \
  X(REG_R14D, "r14d") \
  X(REG_R15D, "r15d")

#define REGISTER_ID_16_LIST \
  X(REG_AX, "ax") \
  X(REG_BX, "bx") \
  X(REG_CX, "cx") \
  X(REG_DX, "dx") \
  X(REG_SI, "si") \
  X(REG_DI, "di") \
  X(REG_BP, "bp") \
  X(REG_SP, "sp") \
  X(REG_R8W, "r8w") \
  X(REG_R9W, "r9w") \
  X(REG_R10W, "r10w") \
  X(REG_R11W, "r11w") \
  X(REG_R12W, "r12w") \
  X(REG_R13W, "r13w") \
  X(REG_R14W, "r14w") \
  X(REG_R15W, "r15w")

#define REGISTER_ID_8_LOWER_LIST \
  X(REG_AL, "al") \
  X(REG_BL, "bl") \
  X(REG_CL, "cl") \
  X(REG_DL, "dl") \
  X(REG_SIL, "sil") \
  X(REG_DIL, "dil") \
  X(REG_BPL, "bpl") \
  X(REG_SPL, "spl") \
  X(REG_R8B, "r8b") \
  X(REG_R9B, "r9b") \
  X(REG_R10B, "r10b") \
  X(REG_R11B, "r11b") \
  X(REG_R12B, "r12b") \
  X(REG_R13B, "r13b") \
  X(REG_R14B, "r14b") \
  X(REG_R15B, "r15b")

#define REGISTER_ID_8_HIGHER_LIST \
  X(REG_AH, "ah") \
  X(REG_BH, "bh") \
  X(REG_CH, "ch") \
  X(REG_DH, "dh")

#define ARITHMETIC_REGISTERS_LIST \
  REGISTER_ID_64_LIST \
  REGISTER_ID_32_LIST \
  REGISTER_ID_16_LIST \
  REGISTER_ID_8_LOWER_LIST \
  REGISTER_ID_8_HIGHER_LIST

#define INSTRUCTION_POINTER X(REG_RIP, "rip")

#define SEGMENT_REGISTERS \
  X(REG_CS, "cs")  /* Code Segment */ \
  X(REG_DS, "ds")  /* Data Segment */ \
  X(REG_ES, "es")  /* Extra Segment */ \
  X(REG_FS, "fs")  /* Additional Segment */ \
  X(REG_GS, "gs")  /* Additional Segment */ \
  X(REG_SS, "ss")  /* Stack Segment */

#define FLAGS_REGISTERS \
  X(REG_RFLAGS, "rflags")  /* Full 64-bit flags register */ \
  X(REG_EFLAGS, "eflags")  /* Lower 32 bits of the flags */ \
  X(REG_FLAGS, "flags")    /* Lower 16 bits of the flags */

#define CONTROL_REGISTERS \
  X(REG_CR0, "cr0")  /* Control Register 0 */ \
  X(REG_CR2, "cr2")  /* Control Register 2 */ \
  X(REG_CR3, "cr3")  /* Control Register 3 */ \
  X(REG_CR4, "cr4")  /* Control Register 4 */ \
  X(REG_CR8, "cr8")  /* Control Register 8 */

#define DEBUG_REGISTERS \
  X(REG_DR0, "dr0")  /* Debug Register 0 */ \
  X(REG_DR1, "dr1")  /* Debug Register 1 */ \
  X(REG_DR2, "dr2")  /* Debug Register 2 */ \
  X(REG_DR3, "dr3")  /* Debug Register 3 */ \
  X(REG_DR6, "dr6")  /* Debug Register 6 */ \
  X(REG_DR7, "dr7")  /* Debug Register 7 */

#define MODEL_SPECIFIC_REGISTERS \
  X(REG_MSR, "msr")  /* Model Specific Register (generic placeholder) */

#define TEST_REGISTERS
  /* x86 test registers are deprecated and not used in x86-64 */

#define ALL_REGISTERS \
  ARITHMETIC_REGISTERS_LIST \
  INSTRUCTION_POINTER \
  SEGMENT_REGISTERS \
  FLAGS_REGISTERS \
  CONTROL_REGISTERS \
  DEBUG_REGISTERS \
  MODEL_SPECIFIC_REGISTERS \
  TEST_REGISTERS

enum class Register: uint32_t
{
#define X(reg, str) reg,
    ALL_REGISTERS
#undef X
_COUNT
};

enum class GeneralPurposeRegister: uint32_t {
#define X(reg, str) reg,
REGISTER_ID_64_LIST
#undef X
_COUNT,
};

#define X(reg, str)                                                            \
  static_assert(                                                               \
      static_cast<uint32_t>(Register::reg) ==                                  \
          static_cast<uint32_t>(GeneralPurposeRegister::reg),                  \
      "Please keep the general purpose registers at the start of the Register & GeneralPurposeRegister enum. " STRINGIFY(reg) " did not match");
REGISTER_ID_64_LIST
#undef X

inline const char *regToStr(Register reg)
{
  switch (reg)
  {
#define X(reg, str) case Register::reg: return str;
    ALL_REGISTERS
#undef X
  default:
    THROW("Register type not found " << ((uint32_t) reg));
  }
}

inline Register strToReg(std::string_view str) {
  static const std::unordered_map<std::string_view, Register> strToRegMap = {
#define X(reg, str) {str, Register::reg},
    ALL_REGISTERS
#undef X
  };

  auto it = strToRegMap.find(str);
  if (it != strToRegMap.end()) {
    return it->second;
  } else {
    THROW("Register string not found: " << str); // TODO make this a USER_THROW or catch it in the parser
  }
}

inline std::ostream& operator<<(std::ostream& os, const Register token) { return os << regToStr(token); }

// To change later proably?
constexpr auto returnRegister = scopes::Register::REG_RBX;

} /* namespace scopes */
