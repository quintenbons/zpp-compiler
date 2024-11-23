#pragma once

#include <cstddef>

#include "nodes.hpp"
#include "scopes/scopeStack.hpp"

namespace ast
{

class DecoratorEvaluator
{
public:
  DecoratorEvaluator(scopes::ScopeStack &scopeStack)
  : _scopeStack{scopeStack}
  , _currentScope{_scopeStack.rootScope()}
  {
  }

  scopes::Scope getCurrentScope() const
  {
    return _currentScope;
  }

protected:
  scopes::ScopeStack &_scopeStack;
  scopes::Scope &_currentScope;
};


} /* namespace ast */
