#pragma once

#include <cstdint>
#include <limits>
#include <vector>
#include <map>
#include <sstream>

#include "dbg/logger.hpp"
#include "types.hpp"
#include "memory_x86_64.hpp"

namespace scopes
{

using scopeId_t = uint32_t;
static constexpr scopeId_t SCOPE_NONE = std::numeric_limits<scopeId_t>::max();

class Scope
{
public:
  Scope(scopeId_t scopeId, Scope *parent): _id{scopeId}, _parent{parent} {}

  void addType(std::unique_ptr<TypeDescription> &&description)
  {
    _types.emplace(description->name, std::move(description));
  }

  void addVariable(std::unique_ptr<VariableDescription> &&description)
  {
    _variables.emplace(description->name, std::move(description));
  }

  const TypeDescription* findType(std::string_view name)
  {
    auto typeDescription = _types.find(name);
    if (typeDescription != _types.end()) return typeDescription->second.get();
    if (!_parent) THROW("Type not found: " << name << " in scope " << _id);
    return _parent->findType(name);
  }

  const VariableDescription* findVariable(std::string_view name)
  {
    auto variableDescription = _variables.find(name);
    if (variableDescription != _variables.end()) return variableDescription->second.get();
    if (!_parent) THROW("Variable not found: " << name << " in scope " << _id);
    return _parent->findVariable(name);
  }

  void logDebug()
  {
    std::stringstream ss;
    ss << "[Scope] id=" << _id << " ; parent=" << (_parent ? _parent->_id : 0);
    for (auto &[name, description]: _types)
    {
      ss << "\n  [Type] id=" << description->id << " ; name=" << description->name << " ; size=" << description->byteSize;
    }
    for (auto &[name, description]: _variables)
    {
      ss << "\n  [Variable] id=" << description->variableId << " ; name=" << description->name << " ; location=";
      std::visit([&ss](auto &&arg) { ss << arg; }, description->location);
    }
    LOG_DEBUG(ss.str());
  }

private:
  scopeId_t _id;
  Scope *_parent; // TODO think about relacing this with a scope id
  std::map<std::string_view, const std::unique_ptr<TypeDescription>> _types;
  std::map<std::string_view, const std::unique_ptr<VariableDescription>> _variables; // TODO lvalues?
};

class ScopeStack
{
public:
  ScopeStack()
  : _scopes{}
  , _types{generatePrimitiveTypeVector()}
  , _stackOffset{0}
  {
    Scope &rootScope = createChildScope(nullptr);

    for (const TypeDescription &description: _types)
    {
      rootScope.addType(std::make_unique<TypeDescription>(description));
    }
  }

  Scope &rootScope() { return _scopes[0]; }

  void logDebug()
  {
    LOG_DEBUG("ScopeStack of " << _scopes.size() << " scopes");
    for (auto &scope: _scopes) scope.logDebug();
  }

  void addLocalVariable(const std::string_view &name, const byteSize_t &size)
  {
    // Let's say size = 32bits for now
    _stackOffset += size;
    std::unique_ptr<VariableDescription> description = std::make_unique<VariableDescription>(VariableDescription{
      .variableId=_variableId++,
      .name=name,
      .location=LocalStackOffset{size, _stackOffset},
    });
    _scopes.back().addVariable(std::move(description));
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
  byteSize_t _stackOffset;
  variableId_t _variableId;
};

} /* namespace scopes */
