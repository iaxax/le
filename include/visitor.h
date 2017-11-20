#ifndef LOOP_EXTRACTION_VISITOR_H
#define LOOP_EXTRACTION_VISITOR_H

#include "rose.h"
#include "variable.h"
#include "loop.h"
#include <string>

namespace LE {

  class VariableTable;

  // check whether an AST represents a constant
  class ConstantCheckVisitor: public AstSimpleProcessing {
  private:
    bool isConstant;
    VariableTable* varTbl;

  protected:
    void virtual visit(SgNode* node);

  public:
    ConstantCheckVisitor(VariableTable* tbl):
      isConstant(true), varTbl(tbl) {}
    inline bool isExprConstant() {return isConstant;}
  };

  // collect all variables in an AST
  class VariableCollector: public AstSimpleProcessing {
  private:
    Loop* loop;

  protected:
    void virtual visit(SgNode* node);

  public:
    VariableCollector(Loop* l): loop(l) {}
  };

  // print an AST
  class PrintVisitor: public AstSimpleProcessing {
  protected:
    void virtual visit(SgNode *node);
  };
}

#endif
