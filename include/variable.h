#ifndef LOOP_EXTRACTION_VARIABLE_H
#define LOOP_EXTRACTION_VARIABLE_H

#include "rose.h"
#include <string>
#include <map>

namespace LE {

  class VariableTable;

  class Variable {
  private:
    VariableTable* parent;
    std::string name;
    SgExpression* value;
    bool isConstant;

  public:
    Variable(std::string &n, SgExpression* v, VariableTable* tbl);

    inline std::string getName() const {return this->name;}
    inline SgExpression* getValue() const {return this->value;}
    inline bool isVarConstant() const {return this->isConstant;}
  };

  class VariableTable {
  private:
    std::map<std::string, Variable*> table;

  public:
    bool isVarConstant(const std::string &name) const;
    void addVariable(Variable* var);
    void addVariable(const std::string &name, Variable* var);
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
