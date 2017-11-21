#include "visitor.h"

namespace LE {

  void VariableCollector::visit(SgNode *node) {
    if (SgVarRefExp* var = dynamic_cast<SgVarRefExp*>(node)) {
      loop->addVariable(var->get_symbol()->get_name().getString());
    } else if (SgInitializedName* var = dynamic_cast<SgInitializedName*>(node)) {
      loop->addVariable(var->get_name().getString());
    }
  }

}
