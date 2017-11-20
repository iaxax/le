#include "variable.h"
#include "visitor.h"

namespace LE {

  Variable::Variable(std::string &n, SgExpression *v, VariableTable *tbl)
      : parent(tbl), name(n), value(v) {
    ConstantCheckVisitor visitor(tbl);
    visitor.traverse(v, preorder);
    isConstant = visitor.isExprConstant();
  }

  bool VariableTable::isVarConstant(const std::string &name) const {
    auto iter = table.find(name);
    return iter != table.end() && iter->second != nullptr &&
           iter->second->isVarConstant();
  }

  void VariableTable::addVariable(Variable *var) {
    std::string name = var->getName();
    table[name] = var;
  }

  void VariableTable::addVariable(const std::string &name, Variable *var) {
    table[name] = var;
  }

  Variable* VariableTable::getVariable(const std::string &name) const {
    auto iter = table.find(name);
    return iter == table.end() ? nullptr : iter->second;
  }

  VariableTable* VariableTable::clone() const {
    VariableTable* result = new VariableTable;
    result->table.insert(table.begin(), table.end());
    return result;
  }

}
