#pragma once

#include <bitset>
#include <optional>

#include "ast/scopes/registers.hpp"
#include "dbg/errors.hpp"

namespace scopes
{

class GPRegisterSet {
public:
  struct RegisterGuard {
    RegisterGuard(GPRegisterSet &set, GeneralPurposeRegister reg): set(set), reg(reg) {}
    ~RegisterGuard() { set.release(reg); }

    GPRegisterSet &set;
    GeneralPurposeRegister reg;
  };

  static constexpr size_t GP_REGISTER_COUNT = static_cast<size_t>(GeneralPurposeRegister::_COUNT);

public:
  std::optional<RegisterGuard> acquireGuard() {
    auto reg = acquire();
    if (!reg) return std::nullopt;

    return RegisterGuard(*this, *reg);
  };

  void release(GeneralPurposeRegister reg) {
    size_t idx = static_cast<size_t>(reg);
    DEBUG_ASSERT(takenRegisters.test(idx), "Tried to release general purpose register that was not acquired (double release?)");
    takenRegisters.reset();
  }

  const std::bitset<GP_REGISTER_COUNT> &asBistet() const { return takenRegisters; }

protected:
  std::optional<GeneralPurposeRegister> acquire() {
    for (size_t i = 0; i < takenRegisters.size(); ++i) {
      if (!takenRegisters.test(i)) {
        takenRegisters.set(i);
        return static_cast<GeneralPurposeRegister>(i);
      }
    }
    return std::nullopt;
  };

private:
  std::bitset<GP_REGISTER_COUNT> takenRegisters;
};



} /* namespace scopes */
