#include "printer.h"
#include "astHelper.h"
#include "message.h"
#include <cassert>
#include <set>

namespace LE {

  // 4 spaces
  const std::string Printer::indent = "    ";

  void Printer::printExpression(std::ostream& os, SgExpression* expr) {
    // if expr is a constant, print its value
    if (SgValueExp* value = dynamic_cast<SgValueExp*>(expr)) {
      os << value->get_constant_folded_value_as_string();
      return;
    }

    // if expr is a variable, print its name
    if (SgVarRefExp* var = dynamic_cast<SgVarRefExp*>(expr)) {
      os << var->get_symbol()->get_name().getString();
      return;
    }

    // if expr is a binary expression
    if (SgBinaryOp* binOp = dynamic_cast<SgBinaryOp*>(expr)) {
      // array expression is handled differently from other binary operator
      if (SgPntrArrRefExp* arrRef = dynamic_cast<SgPntrArrRefExp*>(binOp)) {
        printExpression(os, arrRef->get_lhs_operand());
        os << "[";
        printExpression(os, arrRef->get_rhs_operand());
        os << "]";
      } else {
        os << "(";
        printExpression(os, binOp->get_lhs_operand());
        os << " " << ASTHelper::getOperatorString(binOp->variantT()) << " ";
        printExpression(os, binOp->get_rhs_operand());
        os << ")";
      }
    } else if (SgUnaryOp* unaryOp = dynamic_cast<SgUnaryOp*>(expr)) {
      // unary operation
      os << "(";
      if (dynamic_cast<SgPlusPlusOp*>(unaryOp)) {
        // ++
        printExpression(os, unaryOp->get_operand());
        os << " + 1";
      } else if (dynamic_cast<SgMinusMinusOp*>(unaryOp)) {
        // --
        printExpression(os, unaryOp->get_operand());
        os << " - 1";
      } else if (SgCastExp* castExp = dynamic_cast<SgCastExp*>(unaryOp)) {
        // cast
        printExpression(os, castExp->get_operand());
      } else {
        os << ASTHelper::getOperatorString(unaryOp->variantT());
        printExpression(os, unaryOp->get_operand());
      }
      os << ")";
    } else if (SgAssignInitializer* initializer = dynamic_cast<SgAssignInitializer*>(expr)) {
      // assign initializer
      SgExpression* operand = initializer->get_operand();
      printExpression(os, operand);
    } else {
      std::stringstream ss;
      ss << expr->class_name() << " unsupported in Printer::printExpression\n";
      Message::warning(ss.str());
    }
  }

  void Printer::printVariableUpdate(std::ostream& os, VariableTable* varTbl) {
    os << indent << "path: {";

    // if there are no variables
    auto it = varTbl->begin(), ie = varTbl->end();
    if (it == ie) {
      os << "}\n";
      return;
    }

    // if there are more than one variables
    for (; it != ie; ++it) {
      os << " " << it->first << ": ";
      if (it->second != nullptr) {
        printExpression(os, it->second->getValue());
      }
      if (++it == ie) {
        --it;
        os << " }\n";
      } else {
        --it;
        os << " ,";
      }
    }
  }

  void Printer::printConstraint(std::ostream& os, ConstraintList* cl) {
    os << indent << "constraint: ";
    auto it = cl->begin(), ie = cl->end();
    assert(it != ie);
    for (; it != ie; ++it) {
      if (++it == ie) {
        printExpression(os, *--it);
        os << "\n";
      } else {
        printExpression(os, *--it);
        os << " && ";
      }
    }
  }

  void Printer::printInnerLoops(std::ostream& os, const std::set<std::string>& loopNames) {
    os << indent << "inner loop: {";
    if (loopNames.empty()) {
      os << "}\n";
      return;
    }

    for (auto it = loopNames.begin(), ie = loopNames.end(); it != ie; ++it) {
      if (++it == ie) {
        os << " [" << *--it << "] }\n";
      } else {
        os << " [" << *--it << "],";
      }
    }
  }

  void Printer::printPath(std::ostream& os, LoopPath* path) {
    printVariableUpdate(os, path->getVariableTable());
    printInnerLoops(os, path->getInnerLoops());
    printConstraint(os, path->getConstraintList());

    os << indent << "break: ";
    os << (path->canBreakLoop() ? "true" : "false") << "\n";
  }

  void Printer::printVarInvolved(std::ostream& os,
                                 const std::set<std::string>& variables) {
    os << "Loop variables: [";

    // if there are no variables
    auto vi = variables.begin(), ve = variables.end();
    if (vi == ve) {
      os << "]\n";
      return;
    }

    // if there are more than one variables
    for (; vi != ve; ++vi) {
      if (++vi == ve) {
        os << " '" << *--vi << "' ]\n";
      } else {
        os << " '" << *--vi << "',";
      }
    }
  }

  void Printer::print(std::ostream& os, Loop *loop) {
    os << "==================================================\n";
    // print name
    os << "Loop name: " << loop->getName() << "\n";

    // print variables
    const std::set<std::string>& variables = loop->getAllVariables();
    printVarInvolved(os, variables);

    // print body
    os << "Loop body:\n";
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      printPath(os, *it);
      os << "\n";
    }

    os << "==================================================\n";
    os.flush();
  }

}
