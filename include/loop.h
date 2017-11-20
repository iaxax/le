#ifndef LOOP_EXTRACTION_LOOP_H
#define LOOP_EXTRACTION_LOOP_H

#include "rose.h"
#include "loop.h"
#include "variable.h"
#include "constraint.h"
#include <set>
#include <map>
#include <string>

namespace LE {

  class LoopPath;

  // a loop consists of a name, a set of varialbes
  // and a set of paths
  class Loop {
  private:
    // name of loop (global unique)
    std::string name;

    // variables involved in the loop
    std::set<std::string> variableInvolved;

    // paths of loop
    std::set<LoopPath*> paths;

  public:
    Loop(const std::string& n): name(n) {}
    inline void addPath(LoopPath* p) {paths.insert(p);}
    inline void addVariable(std::string n) { variableInvolved.insert(n); }
    inline std::string getName() {return name;}

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
    // note; not need to clone variableInvolved
    Loop* cloneWithoutBreak() const;

    // merge paths in loop and delete loop
    // note: merged loop must have same name as that of merging loop
    void merge(Loop* loop);
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
    std::set<const Loop*> innerLoops;

    // whether this path can break the loop
    bool canBreak;

  public:
    LoopPath(VariableTable* tbl, ConstraintList* cl, bool b):
      varTbl(tbl), constraintList(cl), canBreak(b) {}

    inline VariableTable* getVariableTable() { return varTbl; }
    inline bool canBreakLoop() const { return canBreak; }
    inline void setCanBreak(bool b) { canBreak = b; }
    inline ConstraintList* getConstraintList() { return constraintList; }

    inline void addConstraint(SgExpression* c) {
      constraintList->addConstraint(c);
    }

    // a deep clone of path
    LoopPath* clone() const {
      return new LoopPath(varTbl->clone(), constraintList->clone(), canBreak);
    }
  };

}

#endif
