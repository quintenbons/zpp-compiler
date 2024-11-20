#pragma once

#include <cstdint>
#include <limits>
#include <vector>
#include <string>
#include <map>
#include <sstream>

#include "dbg/logger.hpp"
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

  void logDebug()
  {
    std::stringstream ss;
    ss << "[Scope] id=" << _id << " ; parent=" << (_parent ? _parent->_id : 0);
    for (auto &[name, description]: _types)
    {
      ss << "\n  [Type] id=" << description.id << " ; name=" << description.name << " ; size=" << description.size;
    }
    LOG_DEBUG(ss.str());
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

  void logDebug()
  {
    LOG_DEBUG("ScopeStack of " << _scopes.size() << " scopes");
    for (auto &scope: _scopes) scope.logDebug();
  }

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
