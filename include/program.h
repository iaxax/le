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
    std::string name;
    VariantT type;
    SgExpression* initValue;
    SgExpression* value;

  public:
    Variable(VariantT ty, const std::string &n, SgExpression* v):
      name(n), type(ty), initValue(v), value(v) {}

    Variable(const std::string &n, SgExpression* v):
      name(n), value(v) {}

    inline std::string getName() { return name; }
    inline SgExpression* getValue() { return value; }
    inline SgExpression* getInitValue() { return initValue; }
    inline VariantT getType() { return type;}
  };

  // a variable table is a set of variables
  class VariableTable {
  private:
    std::map<std::string, Variable*> table;

  public:
    VariableTable() {}

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
    // set of variables, see VariableTable for more detail
    VariableTable* varTbl;

    // a list of constraints, see ConstraintList for more detail
    ConstraintList* constraintList;

    // a set of inner loops
    std::set<std::string> innerLoops;

    // whether this path can break the loop
    bool canBreak;

  public:
    LoopPath(VariableTable* tbl, ConstraintList* cl, bool b):
      varTbl(tbl), constraintList(cl), canBreak(b) {}

    //inline Loop* getParent() { return parent; }
    inline VariableTable* getVariableTable() { return varTbl; }
    inline bool canBreakLoop() const { return canBreak; }
    inline void setCanBreak(bool b) { canBreak = b; }
    inline ConstraintList* getConstraintList() { return constraintList; }
    inline void addInnerLoop(const std::string& loop) { innerLoops.insert(loop); }
    inline const std::set<std::string>& getInnerLoops() const { return innerLoops; }
    inline void addConstraint(SgExpression* c) { constraintList->addConstraint(c); }

    // a deep clone of path
    LoopPath* clone() const {
      // return new LoopPath(varTbl->clone(),
      //   constraintList->clone(), parent, canBreak);
      return new LoopPath(varTbl->clone(), constraintList->clone(), canBreak);
    }
  };

  // a loop consists of a name, a set of varialbes
  // and a set of paths
  class Loop {
  private:

    // name of loop (global unique)
    std::string name;

    // variables declared in the loop
    VariableTable* varTbl;

    // set of inner loops
    std::set<Loop*> innerLoops;

    // paths of loop
    std::set<LoopPath*> paths;

  public:
    Loop(const std::string& n, VariableTable* vt): name(n), varTbl(vt) {}
    inline void addPath(LoopPath* p) {paths.insert(p);}
    inline std::string getName() {return name;}
    inline VariableTable* getVariableTable() { return varTbl; }
    void addInnerLoop(Loop* loop);
    inline const std::set<Loop*>& getInnerLoops() const { return innerLoops; }
    inline std::set<LoopPath*> getPaths() { return paths; }

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

  // a blocks contains update of a set of variables
  class Block {
  private:
    std::string name;
    VariableTable* varTbl;

  public:
    Block(const std::string& n, VariableTable* vt): name(n), varTbl(vt) {}

    inline VariableTable* getVariableTable() { return varTbl; }
    inline std::string getName() { return name; }
  };

  // a path contains a list of constraints,
  // a list of basic blocks, a name and a return value
  class Path {
  private:
    std::string name;
    ConstraintList* constraintList;
    std::vector<std::string> paths;
    SgExpression* retVal;
    bool isReturn;

  public:
    Path(const std::string& n, ConstraintList* cl, SgExpression* rv = nullptr):
      name(n), constraintList(cl), retVal(rv), isReturn(false) {}

    Path(const std::string& n, ConstraintList* cl,
        const std::vector<std::string>& p, SgExpression* rv, bool ret):
          name(n), constraintList(cl), paths(p), retVal(rv), isReturn(ret) {}

    inline std::string getName() { return name; }
    inline SgExpression* getReturnValue() { return retVal; }
    inline void setReturnValue(SgExpression* rv) { retVal = rv; }
    inline bool isPathReturn() { return isReturn; }
    inline void setPathReturn(bool b) { isReturn = b; }
    inline void addPath(const std::string& p) { paths.push_back(p); }
    inline std::vector<std::string> getPaths() { return paths; }
    inline void addConstraint(SgExpression* e) { constraintList->addConstraint(e); }
    inline ConstraintList* getConstraints() { return constraintList; }


    Path* clone();
  };

  // a function contains a name, a set of parameters,
  // a set of variables, a set of paths and a return value
  //
  // note: loops here describes detail infomration of loops in paths
  //       blocks here describes details information of blocks in paths
  class Function {
  private:
    std::string name;
    std::set<std::string> parameters;
    VariableTable* varTbl;
    std::set<Path*> paths;
    std::set<Loop*> loops;
    std::set<Block*> blocks;

  public:
    Function(VariableTable* vt): varTbl(vt) {}

    inline std::string getName() { return name; }
    inline void setName(const std::string& n) { name = n; }
    inline void addParam(const std::string& p) { parameters.insert(p); }
    inline void addVariable(Variable* var) { varTbl->addVariable(var); }
    inline VariableTable* getVariableTable() { return varTbl; }
    inline std::set<std::string> getParams() { return parameters; }
    inline void addPath(Path* p) { paths.insert(p); }
    inline std::set<Path*> getPaths() { return paths; }
    inline void addLoop(Loop* l) { loops.insert(l); }
    inline void addBlock(Block* b) { blocks.insert(b); }
    inline std::set<Block*> getBlocks() { return blocks; }
    inline std::set<Loop*> getLoops() { return loops; }

    // clone a function with the same paths as origin function except
    // paths that have been returned
    Function* cloneNotReturnPaths();
    void merge(Function* func);
  };

  // a program corresbonds to a source file
  // a program contains a name, a set of global variables
  // and a set of functions
  class Program {
  private:
    std::string name;
    VariableTable* varTbl;
    std::set<Function*> globalFuncs;

  public:
    Program(const std::string& n, VariableTable* vt):
      name(n), varTbl(vt) {}

    inline VariableTable* getVariableTable() { return varTbl; }
    inline std::string getName() { return name; }
    inline std::set<Function*> getFunctions() { return globalFuncs; }
    inline void addFunction(Function* func) { globalFuncs.insert(func); }
  };

}

#endif
