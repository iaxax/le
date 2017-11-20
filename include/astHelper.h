#ifndef LOOP_EXTRACTION_AST_HELPER_H
#define LOOP_EXTRACTION_AST_HELPER_H

#include "rose.h"
#include <string>
#include <functional>

namespace LE {

  class ASTHelper {
  public:
    // deep clone of an AST
    static SgNode* clone(SgNode* node);

    // traverse AST to replace every occurrence of
    // variable having the given name with a new value
    static void replaceVar(SgNode* tree,
      SgExpression* newValue, const std::string& name);

    // type must be subtype of SgCompoundAssignOp
    // with given SgCompoundAssignOp, return corresbonding binary operator
    // e.g. SgPlusAssignOp -> SgAddOp, SgMinusAssignOp -> SgMinusOp
    static SgBinaryOp* toBinaryOp(VariantT type, SgExpression* lhs,
      SgExpression* rhs, SgType* exprType);

    static std::string getOperatorString(VariantT type);
  };

}

#endif
