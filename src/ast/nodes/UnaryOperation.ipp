#include "ast/nodes/nodes.h"

namespace ast {

UnaryOperation::UnaryOperation(Operation op, std::unique_ptr<Expression> &&expr)
    : op(op), expr(std::move(expr)) {}

UnaryOperation::UnaryOperation(const UnaryOperation &other)
    : op(other.op), expr(std::make_unique<Expression>(*other.expr)) {}

inline void UnaryOperation::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    (void)generator;
    THROW("BinaryOperation genAsm_x86_64 should not be called");
}

inline void UnaryOperation::debug(size_t depth) const {
    logNode(depth, static_cast<char>(op));
    expr->debug(depth+1);
}

inline void UnaryOperation::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    expr->decorate(scopeStack, scope);
}

inline void UnaryOperation::loadValueInRegister(codegen::NasmGenerator_x86_64 &generator, scopes::GeneralPurposeRegister targetRegister) const {
    auto properTargetReg = scopes::getProperRegisterFromID64(targetRegister, scopes::ADDRESS_SIZE);
    switch (op)
    {
    case Operation::ADDRESS_OF:
        generator.emitLoadAddress(properTargetReg, expr->getMemoryOffset());
        break;
    default:
        THROW("Unrecognised operation " << static_cast<char>(op));
    }
}

} /* namespace ast */

