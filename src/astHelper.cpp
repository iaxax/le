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
  static std::map<VariantT, std::string> operatorStrMap;
  void initOperatorStringMap() {
    operatorStrMap[V_SgEqualityOp] = "==";
    operatorStrMap[V_SgLessThanOp] = "<";
    operatorStrMap[V_SgGreaterThanOp] = ">";
    operatorStrMap[V_SgNotEqualOp] = "!=";
    operatorStrMap[V_SgLessOrEqualOp] = "<=";
    operatorStrMap[V_SgGreaterOrEqualOp] = ">=";
    operatorStrMap[V_SgAddOp] = "+";
    operatorStrMap[V_SgSubtractOp] = "-";
    operatorStrMap[V_SgMultiplyOp] = "*";
    operatorStrMap[V_SgDivideOp] = "/";
    operatorStrMap[V_SgIntegerDivideOp] = "/";
    operatorStrMap[V_SgModOp] = "%";
    operatorStrMap[V_SgAndOp] = "&&";
    operatorStrMap[V_SgOrOp] = "||";
    operatorStrMap[V_SgBitXorOp] = "^";
    operatorStrMap[V_SgBitAndOp] = "&";
    operatorStrMap[V_SgBitOrOp] = "|";
    operatorStrMap[V_SgCommaOpExp] = ",";
    operatorStrMap[V_SgLshiftOp] = "<<";
    operatorStrMap[V_SgRshiftOp] = ">>";
    operatorStrMap[V_SgNotOp] = "!";
    operatorStrMap[V_SgAssignOp] = "=";
    operatorStrMap[V_SgPointerDerefExp] = "*";
    operatorStrMap[V_SgAddressOfOp] = "&";
    operatorStrMap[V_SgPlusAssignOp] = "+=";
    operatorStrMap[V_SgMinusAssignOp] = "-=";
    operatorStrMap[V_SgAndAssignOp] = "&&=";
    operatorStrMap[V_SgIorAssignOp] = "||=";
    operatorStrMap[V_SgMultAssignOp] = "*=";
    operatorStrMap[V_SgDivAssignOp] = "/=";
    operatorStrMap[V_SgModAssignOp] = "%=";
    operatorStrMap[V_SgXorAssignOp] = "^=";
    operatorStrMap[V_SgLshiftAssignOp] = "<<=";
  }

  // map from int to string form of variable type
  std::map<int, std::string> typeStrMap;
  void initTypeStringMap() {
    typeStrMap[T_CHAR] = "char";
    typeStrMap[T_SIGNED_CHAR] = "singed char";
    typeStrMap[T_UNSIGNED_CHAR] = "unsigned char";
    typeStrMap[T_SHORT] = "short";
    typeStrMap[T_SIGNED_SHORT] = "signed short";
    typeStrMap[T_UNSIGNED_SHORT] = "unsigned short";
    typeStrMap[T_INT] = "int";
    typeStrMap[T_SIGNED_INT] = "signed int";
    typeStrMap[T_UNSIGNED_INT] = "unsigned int";
    typeStrMap[T_LONG] = "long";
    typeStrMap[T_SIGNED_LONG] = "signed long";
    typeStrMap[T_UNSIGNED_LONG] = "unsigned long";
    typeStrMap[T_FLOAT] = "float";
    typeStrMap[T_DOUBLE] = "double";
    typeStrMap[T_LONG_LONG] = "long long";
    typeStrMap[T_SIGNED_LONG_LONG] = "singed long long";
    typeStrMap[T_UNSIGNED_LONG_LONG] = "unsigned long long";
    typeStrMap[T_LONG_DOUBLE] = "long double";
    typeStrMap[T_STRING] = "string";
    typeStrMap[T_BOOL] = "bool";
    typeStrMap[T_ARRAY] = "array";
    typeStrMap[T_POINTER] = "pointer";
  }

  //--------------ASTHelper member function-------------------------//

  // C++ is lack of static block as Java
  // I use this class to initialize static data members
  class Init {
  public:
    Init() {
      initConstructorMap();
      initOperatorStringMap();
      initTypeStringMap();
    }
  };
  static Init init;

  SgBinaryOp* ASTHelper::toBinaryOp(VariantT type, SgExpression* lhs,
    SgExpression* rhs, SgType* exprType) {
    auto iter = constructorMap.find(type);
    assert(iter != constructorMap.end() && "unsupported binary operator");
    return iter->second(lhs, rhs, exprType);
  }

  std::string ASTHelper::getOperatorString(VariantT type) {
    auto iter = operatorStrMap.find(type);
    assert(iter != operatorStrMap.end() && "unsupported binary operator");
    return iter->second;
  }

  std::string ASTHelper::getTypeString(VariantT type) {
    auto iter = typeStrMap.find(type);
    assert(iter != typeStrMap.end() && "unsupported variable type");
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
