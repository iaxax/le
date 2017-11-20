#ifndef LOOP_EXTRACTION_VISITOR_H
#define LOOP_EXTRACTION_VISITOR_H

#include "rose.h"
#include "loop.h"
#include <string>

namespace LE {


  // collect all variables in an AST
  class VariableCollector: public AstSimpleProcessing {
  private:
    Loop* loop;

  protected:
    void virtual visit(SgNode* node);

  public:
    VariableCollector(Loop* l): loop(l) {}
  };

}

#endif
