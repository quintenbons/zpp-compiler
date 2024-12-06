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
private:
friend class ScopeStack;

  void addLocalVariable(const std::string_view &name, const TypeDescription* type, id_t variableId)
  {
    _stackOffset += type->byteSize;
    std::unique_ptr<VariableDescription> description = std::make_unique<VariableDescription>(VariableDescription{
      .variableId=variableId,
      .name=name,
      .location=LocalStackOffset{type->byteSize, _stackOffset},
      .typeDescription=type,
    });
    _variables.emplace(description->name, std::move(description));
  }

  void addType(std::unique_ptr<TypeDescription> &&description)
  {
    _types.emplace(description->name, std::move(description));
  }

  void addFunction(const std::string_view &name, const std::vector<const TypeDescription*> &parameters, const TypeDescription* returnType, id_t functionId)
  {
    std::unique_ptr<FunctionDescription> description = std::make_unique<FunctionDescription>(FunctionDescription{
      .functionId=functionId,
      .name=name,
      .parameters=parameters,
      .returnType=returnType,
    });
    _functions.emplace(description->name, std::move(description));
  }
public:
  Scope(scopeId_t scopeId, Scope *parent): _id{scopeId}, _parent{parent}, _stackOffset{0} {}

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

  const FunctionDescription* findFunction(std::string_view name)
  {
    auto functionDescription = _functions.find(name);
    if (functionDescription != _functions.end()) return functionDescription->second.get();
    if (!_parent) THROW("Function not found: " << name << " in scope " << _id);
    return _parent->findFunction(name);
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
    for (auto &[name, description]: _functions)
    {
      ss << "\n  [Function] name=" << description->name << " ; returnType=" << description->returnType->name << " ; parameters=";
      for (const TypeDescription *param: description->parameters)
      {
        ss << param->name << " ";
      }
    }
    LOG_DEBUG(ss.str());
  }

private:
  scopeId_t _id;
  Scope *_parent; // TODO think about relacing this with a scope id
  byteSize_t _stackOffset;
  std::map<std::string_view, const std::unique_ptr<TypeDescription>> _types;
  std::map<std::string_view, const std::unique_ptr<VariableDescription>> _variables; // TODO lvalues?
  std::map<std::string_view, const std::unique_ptr<FunctionDescription>> _functions;
};

class ScopeStack
{
public:
  ScopeStack()
  : _scopes{}
  , _types{generatePrimitiveTypeVector()}
  , _variableId{0}
  {
    Scope &rootScope = createChildScope(nullptr);

    for (const TypeDescription &description: _types)
    {
      rootScope.addType(std::make_unique<TypeDescription>(description));
    }
  }

  Scope &rootScope() { return *_scopes[0].get(); }

  void logDebug()
  {
    LOG_DEBUG("ScopeStack of " << _scopes.size() << " scopes");
    for (auto &scope: _scopes) scope->logDebug();
  }

  Scope &createChildScope(Scope *parent)
  {
    _scopes.push_back(std::make_unique<Scope>(_scopes.size(), parent));
    return *_scopes.back().get();
  }

  void addLocalVariable(const std::string_view &name, const TypeDescription* type, Scope &scope)
  {
    _variableId++;
    scope.addLocalVariable(name, type, _variableId);
  }

  void addFunction(const std::string_view &name, const std::vector<const TypeDescription*> &parameters, const TypeDescription* returnType, Scope &scope)
  {
    _functionId++;
    scope.addFunction(name, parameters, returnType, _functionId);
  }

private:
  std::vector<std::unique_ptr<Scope>> _scopes;
  std::vector<TypeDescription> _types;
  id_t _variableId;
  id_t _functionId;
};

} /* namespace scopes */
