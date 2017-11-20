#include "astHelper.h"
#include "message.h"
#include <cassert>
#include <sstream>

namespace LE {

  // ------------------------- init data ----------------------------------//

  // map compoundAssign type to binary operator constructor
  // e.g. SgPlusAssignOp -> SgAddOp
  typedef std::map<VariantT, std::function<
    SgBinaryOp*(SgExpression*, SgExpression*, SgType*)> > ConstructorMap;

  static ConstructorMap constructorMap;

  // macro for convenience of adding content to constructorMap
  #define ADD_TYPE_CONSTRUCTOR(variant, BinaryOp) constructorMap[variant] = \
    [](SgExpression* lhs, SgExpression* rhs, SgType* type) \
    { return new BinaryOp(lhs, rhs, type); };

  void initConstructorMap() {
    ADD_TYPE_CONSTRUCTOR(V_SgPlusAssignOp, SgAddOp)
    ADD_TYPE_CONSTRUCTOR(V_SgMinusAssignOp, SgSubtractOp)
    ADD_TYPE_CONSTRUCTOR(V_SgAndAssignOp, SgAndOp)
    ADD_TYPE_CONSTRUCTOR(V_SgIorAssignOp, SgOrOp)
    ADD_TYPE_CONSTRUCTOR(V_SgMultAssignOp, SgMultiplyOp)
    ADD_TYPE_CONSTRUCTOR(V_SgDivAssignOp, SgDivideOp)
    ADD_TYPE_CONSTRUCTOR(V_SgModAssignOp, SgModOp)
    ADD_TYPE_CONSTRUCTOR(V_SgXorAssignOp, SgBitXorOp)
    ADD_TYPE_CONSTRUCTOR(V_SgLshiftAssignOp, SgLshiftOp)
    ADD_TYPE_CONSTRUCTOR(V_SgRshiftAssignOp, SgRshiftOp)
  }

  // map from int to string form of operator
  static std::map<VariantT, std::string> typeStrMap;
  void initTypeStringMap() {
    typeStrMap[V_SgEqualityOp] = "==";
    typeStrMap[V_SgLessThanOp] = "<";
    typeStrMap[V_SgGreaterThanOp] = ">";
    typeStrMap[V_SgNotEqualOp] = "!=";
    typeStrMap[V_SgLessOrEqualOp] = "<=";
    typeStrMap[V_SgGreaterOrEqualOp] = ">=";
    typeStrMap[V_SgAddOp] = "+";
    typeStrMap[V_SgSubtractOp] = "-";
    typeStrMap[V_SgMultiplyOp] = "*";
    typeStrMap[V_SgDivideOp] = "/";
    typeStrMap[V_SgIntegerDivideOp] = "/";
    typeStrMap[V_SgModOp] = "%";
    typeStrMap[V_SgAndOp] = "&&";
    typeStrMap[V_SgOrOp] = "||";
    typeStrMap[V_SgBitXorOp] = "^";
    typeStrMap[V_SgBitAndOp] = "&";
    typeStrMap[V_SgBitOrOp] = "|";
    typeStrMap[V_SgCommaOpExp] = ",";
    typeStrMap[V_SgLshiftOp] = "<<";
    typeStrMap[V_SgRshiftOp] = ">>";
    typeStrMap[V_SgNotOp] = "!";
    typeStrMap[V_SgAssignOp] = "=";
    typeStrMap[V_SgPointerDerefExp] = "*";
    typeStrMap[V_SgAddressOfOp] = "&";
    typeStrMap[V_SgPlusAssignOp] = "+=";
    typeStrMap[V_SgMinusAssignOp] = "-=";
    typeStrMap[V_SgAndAssignOp] = "&&=";
    typeStrMap[V_SgIorAssignOp] = "||=";
    typeStrMap[V_SgMultAssignOp] = "*=";
    typeStrMap[V_SgDivAssignOp] = "/=";
    typeStrMap[V_SgModAssignOp] = "%=";
    typeStrMap[V_SgXorAssignOp] = "^=";
    typeStrMap[V_SgLshiftAssignOp] = "<<=";
  }
  //--------------ASTHelper member function-------------------------//

  // C++ is lack of static block as Java
  // I use this class to initialize static data members
  class Init {
  public:
    Init() {
      initConstructorMap();
      initTypeStringMap();
    }
  };
  static Init init;

  SgBinaryOp* ASTHelper::toBinaryOp(VariantT type, SgExpression* lhs,
    SgExpression* rhs, SgType* exprType) {
    auto iter = constructorMap.find(type);
    assert(iter != constructorMap.end());
    return iter->second(lhs, rhs, exprType);
  }

  std::string ASTHelper::getOperatorString(VariantT type) {
    auto iter = typeStrMap.find(type);
    assert(iter != typeStrMap.end());
    return iter->second;
  }

  void ASTHelper::replaceVar(SgNode* tree,
    SgExpression *newValue, const std::string& name) {
    // if it is a binary operator
    // replace both side (lhs and rhs) with the new value
    // if it is the variable we want
    if (SgBinaryOp *binOp = dynamic_cast<SgBinaryOp*>(tree)) {
      SgExpression* left = binOp->get_lhs_operand();
      // if lhs operand is a variable
      // check whether it is the variable we looking for by its name
      // if it is, replace its value with new value
      // otherwise, ignore it
      if (SgVarRefExp *leftVar = dynamic_cast<SgVarRefExp*>(left)) {
        std::string n = leftVar->get_symbol()->get_name().getString();
        if (name == n) {
          binOp->set_lhs_operand(newValue);
        }
      } else {
        // if it is not a variable, recursively to traverse subtree
        replaceVar(left, newValue, name);
      }

      // do similar thing as lhs operand
      SgExpression* right = binOp->get_rhs_operand();
      if (SgVarRefExp *rightVar = dynamic_cast<SgVarRefExp*>(right)) {
        std::string n = rightVar->get_symbol()->get_name().getString();
        if (name == n) {
          binOp->set_rhs_operand(newValue);
        }
      } else {
        replaceVar(right, newValue, name);
      }
    } else if (SgUnaryOp *uOp = dynamic_cast<SgUnaryOp*>(tree)) {
      // if it is a unary operator
      // do similar thing as binary operator does
      SgExpression* operand = uOp->get_operand();
      if (SgVarRefExp *var = dynamic_cast<SgVarRefExp*>(operand)) {
        std::string n = var->get_symbol()->get_name().getString();
        if (name == n) {
          uOp->set_operand(newValue);
        }
      } else {
        replaceVar(operand, newValue, name);
      }
    }
  }
}
