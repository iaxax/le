#include "loop.h"
#include <cassert>

namespace LE {

  Loop* Loop::cloneWithoutBreak() const {
    Loop* newLoop = new Loop(name);
    for (auto p : paths) {
      if (!p->canBreakLoop()) {
        newLoop->paths.insert(p->clone());
      }
    }
    return newLoop;
  }

  void Loop::merge(Loop* loop) {
    assert(name == loop->name);
    paths.insert(loop->paths.begin(), loop->paths.end());
    variableInvolved.insert(loop->variableInvolved.begin(),
      loop->variableInvolved.end());
    delete loop;
  }

}
