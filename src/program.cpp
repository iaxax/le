#include "program.h"
#include "nameAllocator.h"
#include "astHelper.h"
#include <cassert>

namespace LE {

  Loop* Loop::cloneWithoutBreak() const {
    Loop* newLoop = new Loop(name, varTbl);
    for (auto p : paths) {
      if (!p->canBreakLoop()) {
        newLoop->paths.insert(p->clone());
        newLoop->innerLoops.insert(
          innerLoops.begin(), innerLoops.end()
        );
      }
    }
    return newLoop;
  }

  void Loop::merge(Loop* loop) {
    assert(name == loop->name);

    paths.insert(loop->paths.begin(), loop->paths.end());
    innerLoops.insert(loop->innerLoops.begin(),
      loop->innerLoops.end());

    delete loop;
  }

  void Loop::addInnerLoop(Loop* loop) {
    innerLoops.insert(loop);
    const std::string& name = loop->getName();
    for (LoopPath* path : paths) {
      if (!path->canBreakLoop()) {
        path->addInnerLoop(name);
      }
    }
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

  Path* Path::clone() {
    ConstraintList* cl = constraintList->clone();
    std::string&& pathName = PathNameAllocator::allocName();
    SgExpression* rv = ASTHelper::clone(retVal);
    return new Path(pathName, cl, paths, rv, isReturn);
  }

  Function* Function::cloneNotReturnPaths() {
    Function* newFunc = new Function(varTbl);
    for (Path* p : paths) {
      if (!p->isPathReturn()) {
        newFunc->addPath(p->clone());
      }
    }
    return newFunc;
  }

  void Function::merge(Function* func) {
    for (Path* p : func->getPaths()) {
      paths.insert(p);
    }
    for (Block* b : func->getBlocks()) {
      blocks.insert(b);
    }
    for (Loop* l : func->getLoops()) {
      loops.insert(l);
    }

    delete func;
  }

}
