#include "loop.h"
#include <cassert>

namespace LE {

  Loop* Loop::cloneWithoutBreak() const {
    Loop* newLoop = new Loop(name, parent);
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
    assert(parent == loop->parent);

    paths.insert(loop->paths.begin(), loop->paths.end());
    variableInvolved.insert(loop->variableInvolved.begin(),
      loop->variableInvolved.end());
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
}
