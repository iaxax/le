#ifndef LOOP_EXTRACTION_LOOP_H
#define LOOP_EXTRACTION_LOOP_H

#include "rose.h"
#include "constraint.h"
#include <set>
#include <map>
#include <string>

namespace LE {

  class VariableTable;
  class LoopPath;
  class Loop;

  // a variable consists of a name, a value
  // it belongs to a variable table
  class Variable {
  private:
    VariableTable* parent;
    std::string name;
    SgExpression* value;

  public:
    Variable(const std::string &n, SgExpression *v, VariableTable *tbl)
        : parent(tbl), name(n), value(v) {}

    inline std::string getName() const {return this->name;}
    inline SgExpression* getValue() const {return this->value;}
  };

  // a variable table is a set of variables
  class VariableTable {
  private:
    LoopPath* parent;
    std::map<std::string, Variable*> table;

  public:
    VariableTable(LoopPath* p): parent(p) {}
    VariableTable() {}

    inline LoopPath* getParent() { return parent; }
    inline void setParent(LoopPath* p) { parent = p; }

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

  // a path consists of a variable table,
  // a list of constraints, a set of loops
  // there are two kinds of paths
  // one can break the loop and the other can not
  class LoopPath {
  private:
    Loop* parent;

    // set of variables, see VariableTable for more detail
    VariableTable* varTbl;

    // a list of constraints, see ConstraintList for more detail
    ConstraintList* constraintList;

    // a set of inner loops
    std::set<std::string> innerLoops;

    // whether this path can break the loop
    bool canBreak;

  public:
    LoopPath(VariableTable* tbl, ConstraintList* cl, Loop* l, bool b):
      parent(l), varTbl(tbl), constraintList(cl), canBreak(b) {}

    inline Loop* getParent() { return parent; }
    inline VariableTable* getVariableTable() { return varTbl; }
    inline bool canBreakLoop() const { return canBreak; }
    inline void setCanBreak(bool b) { canBreak = b; }
    inline ConstraintList* getConstraintList() { return constraintList; }
    inline void addInnerLoop(const std::string& loop) { innerLoops.insert(loop); }

    inline const std::set<std::string>& getInnerLoops() const {
      return innerLoops;
    }

    inline void addConstraint(SgExpression* c) {
      constraintList->addConstraint(c);
    }

    // a deep clone of path
    LoopPath* clone() const {
      return new LoopPath(varTbl->clone(),
        constraintList->clone(), parent, canBreak);
    }
  };

  // a loop consists of a name, a set of varialbes
  // and a set of paths
  class Loop {
  private:
    // a loop may be inner loop of another loop
    // the most outside loop has no parent(nullptr)
    Loop* parent;

    // name of loop (global unique)
    std::string name;

    // variables involved in the loop
    std::set<std::string> variableInvolved;

    // set of inner loops
    std::set<Loop*> innerLoops;

    // paths of loop
    std::set<LoopPath*> paths;

  public:
    Loop(const std::string& n, Loop* p): parent(p), name(n) {}
    inline void addPath(LoopPath* p) {paths.insert(p);}
    inline void addVariable(std::string n) { variableInvolved.insert(n); }
    inline bool containVariable(const std::string &name) {
      return variableInvolved.find(name) != variableInvolved.end();
    }

    inline std::string getName() {return name;}
    inline Loop* getParent() { return parent; }
    void addInnerLoop(Loop* loop);

    inline const std::set<Loop*>& getInnerLoops() const {
      return innerLoops;
    }

    inline const std::set<std::string>& getAllVariables() const {
      return variableInvolved;
    }

    typedef std::set<LoopPath*>::iterator iterator;
    typedef std::set<LoopPath*>::const_iterator const_iterator;

    inline iterator begin() { return paths.begin(); }
    inline iterator end() { return paths.end(); }
    inline const_iterator begin() const { return paths.begin(); }
    inline const_iterator end() const { return paths.end(); }

    // deep clone of a loop
    // except paths that break the loop
    // note: not need to clone variableInvolved
    Loop* cloneWithoutBreak() const;

    // merge paths in loop and delete loop
    // note: merged loop must have same name as that of merging loop
    void merge(Loop* loop);
  };

  // a function contains a name, a set of variables,
  // a set of loops and a return value
  class Function {

  };

  // a program corresbonds to a source file
  // a program contains a name, a set of global variables
  // and a set of functions
  class Program {
  private:
    std::string name;
    std::set<Variable*> globalVars;
    std::set<Function*> globalFuncs;
  };

}

#endif
