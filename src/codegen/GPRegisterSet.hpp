#pragma once

#include <bitset>
#include <optional>

#include "ast/scopes/registers.hpp"
#include "dbg/errors.hpp"

namespace scopes
{

class GPRegisterSet {
public:
  static constexpr size_t GP_REGISTER_COUNT = static_cast<size_t>(GeneralPurposeRegister::_COUNT);

public:
  std::optional<Register> acquire() {
    for (size_t i = 0; i < takenRegisters.size(); ++i) {
        if (!takenRegisters.test(i)) {
          takenRegisters.set(i);
          return static_cast<Register>(i);
        }
    }
    return std::nullopt;
  };

  void release(Register reg) {
    size_t idx = static_cast<size_t>(reg);
    DEBUG_ASSERT(takenRegisters.test(idx), "Tried to release general purpose register that was not acquired (double release?)");
    takenRegisters.reset();
  }

  const std::bitset<GP_REGISTER_COUNT> &asBistet() const { return takenRegisters; }

protected:
  size_t findFirstUnSet() {
  }

private:
  std::bitset<GP_REGISTER_COUNT> takenRegisters;
};



} /* namespace scopes */
