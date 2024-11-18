#pragma once

#include <cstdint>
#include <limits>
#include <vector>
#include <string>
#include <map>

#include "types.hpp"

namespace scopes
{

using scopeId_t = uint32_t;
static constexpr scopeId_t SCOPE_NONE = std::numeric_limits<scopeId_t>::max();

class Scope
{
public:
  Scope(scopeId_t scopeId, Scope *parent): _id{scopeId}, _parent{parent} {}

  void addType(const TypeDescription &description)
  {
    _types.insert({std::string_view(description.name), description});
  }

  const TypeDescription &findType(std::string_view name)
  {
    auto typeDescription = _types.find(name);
    if (typeDescription != _types.end()) return typeDescription->second;
    if (!_parent) throw std::exception(); // TODO custom errors
    return _parent->findType(name);
  }

private:
  scopeId_t _id;
  Scope *_parent; // TODO think about relacing this with a scope id
  std::map<std::string_view, const TypeDescription &> _types;
  // std::map<std::string_view, const LocationDescription &> _locations; // TODO lvalues?
};

class ScopeStack
{
public:
  ScopeStack()
  : _scopes{}
  , _types{generatePrimitiveTypeVector()}
  {
    Scope &rootScope = createChildScope(nullptr);

    for (const TypeDescription &description: _types)
    {
      rootScope.addType(description);
    }
  }

  Scope &rootScope() { return _scopes[0]; }

private:
  Scope &createChildScope(Scope *parent)
  {
    _scopes.emplace_back(_scopes.size(), parent);
    return _scopes.back();
  }

private:
  std::vector<Scope> _scopes;
  std::vector<TypeDescription> _types;
};

} /* namespace scopes */
