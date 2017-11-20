#ifndef LOOP_EXTRACTION_AST_HELPER_H
#define LOOP_EXTRACTION_AST_HELPER_H

#include "rose.h"
#include <string>
#include <functional>

namespace LE {

  class ASTHelper {
  public:
    // deep clone of an AST
    // T must be SgNode or subclass of SgNode
    template<class T>
    static T* clone(T* node) {
      SgTreeCopy deepCopy;
      return dynamic_cast<T*>(deepCopy.copyAst(dynamic_cast<SgNode*>(node)));
    }

    // traverse AST to replace every occurrence of
    // variable with a new value
    static void replaceVar(SgNode* tree,
      SgExpression* newValue, const std::string& name);

    // type must be subtype of SgCompoundAssignOp
    // with given SgCompoundAssignOp, return corresbonding binary operator
    // e.g. SgPlusAssignOp -> SgAddOp, SgMinusAssignOp -> SgMinusOp
    static SgBinaryOp* toBinaryOp(VariantT type, SgExpression* lhs,
      SgExpression* rhs, SgType* exprType);

    // map from int value of operator to string
    // e.g. VariantT::V_SgEqualityOp -> '=='
    static std::string getOperatorString(VariantT type);
  };

}

#endif
