#include "visitor.h"
#include "variable.h"
#include <iostream>

namespace LE {

  class VariableTable;

  // --------------------ConstantCheckVisitor--------------------------//
  void ConstantCheckVisitor::visit(SgNode* node) {
    if (!isConstant) return;
    if (dynamic_cast<SgFunctionCallExp*>(node)) {
      isConstant = false;
    } else if (SgVarRefExp* var = dynamic_cast<SgVarRefExp*>(node)) {
      std::string name = var->get_symbol()->get_name().getString();
      if (!varTbl->isVarConstant(name)) {
        isConstant = false;
      }
    }
  }

  // ----------------End of ConstantCheckVisitor----------------------//


  // -----------------------VariableCollector-------------------------//
  void VariableCollector::visit(SgNode *node) {
    if (SgVarRefExp* var = dynamic_cast<SgVarRefExp*>(node)) {
      loop->addVariable(var->get_symbol()->get_name().getString());
    } else if (SgInitializedName* var = dynamic_cast<SgInitializedName*>(node)) {
      loop->addVariable(var->get_name().getString());
    }
  }
  // -------------------End of VariableCollector----------------------//


  // --------------------------PrintVisitor--------------------------//
  void PrintVisitor::visit(SgNode *node) {
    long addr = (long)(void*)node;
    std::cout << addr << ":" << node->class_name() << std::endl;
  }
  // --------------------End of PrintVisitor-------------------------//

}
