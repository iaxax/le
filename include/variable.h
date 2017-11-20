#ifndef LOOP_EXTRACTION_VARIABLE_H
#define LOOP_EXTRACTION_VARIABLE_H

#include "rose.h"
#include <string>
#include <map>

namespace LE {

  class VariableTable;

  // a variable consists of a name, a value
  // it belongs to a variable table
  class Variable {
  private:
    VariableTable* parent;
    std::string name;
    SgExpression* value;

  public:
    Variable(std::string &n, SgExpression *v, VariableTable *tbl)
        : parent(tbl), name(n), value(v) {}

    inline std::string getName() const {return this->name;}
    inline SgExpression* getValue() const {return this->value;}
  };

  // a variable table is a set of variables
  class VariableTable {
  private:
    std::map<std::string, Variable*> table;

  public:
    inline void addVariable(Variable* var) {
      assert(var != nullptr);
      table[var->getName()] = var;
    }

    inline void addVariable(const std::string &name, Variable* var) {
      table[name] = var;
    }

    Variable* getVariable(const std::string &name) const;
    VariableTable* clone() const;

    typedef std::map<std::string, Variable*>::iterator iterator;
    typedef std::map<std::string, Variable*>::const_iterator const_iterator;

    inline iterator begin() { return table.begin(); }
    inline iterator end() { return table.end(); }
    inline const_iterator begin() const { return table.begin(); }
    inline const_iterator end() const { return table.end(); }
  };

}

#endif
