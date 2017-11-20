#include "variable.h"

namespace LE {

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
