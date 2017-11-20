#include "printer.h"
#include "astHelper.h"
#include <cassert>
#include <set>

namespace LE {

  const std::string Printer::indent = "    ";

  void Printer::printExpression(std::ostream& os, SgExpression* expr) {
    if (SgValueExp* value = dynamic_cast<SgValueExp*>(expr)) {
      os << value->get_constant_folded_value_as_string();
      return;
    }

    if (SgVarRefExp* var = dynamic_cast<SgVarRefExp*>(expr)) {
      os << var->get_symbol()->get_name().getString();
      return;
    }

    if (SgBinaryOp* binOp = dynamic_cast<SgBinaryOp*>(expr)) {
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
      os << "(";
      if (dynamic_cast<SgPlusPlusOp*>(unaryOp)) {
        printExpression(os, unaryOp->get_operand());
        os << " + 1";
      } else if (dynamic_cast<SgMinusMinusOp*>(unaryOp)) {
        printExpression(os, unaryOp->get_operand());
        os << " - 1";
      } else {
        os << ASTHelper::getOperatorString(unaryOp->variantT());
        printExpression(os, unaryOp->get_operand());
      }
      os << ")";
    }
  }

  void Printer::printVariableUpdate(std::ostream& os, VariableTable* varTbl) {
    os << indent << "path: { ";
    auto it = varTbl->begin(), ie = varTbl->end();
    if (it == ie) {
      os << "}\n";
    } else {
      for (; it != ie; ++it) {
        os << it->first << ": ";
        if (it->second != nullptr) {
          printExpression(os, it->second->getValue());
        }
        if (++it == ie) {
          --it;
          os << " }\n";
        } else {
          --it;
          os << ", ";
        }
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

  void Printer::printPath(std::ostream& os, LoopPath* path) {
    printVariableUpdate(os, path->getVariableTable());
    printConstraint(os, path->getConstraintList());
    os << indent << "break: ";
    os << (path->canBreakLoop() ? "true" : "false") << "\n";
  }

  void Printer::printVarInvolved(std::ostream& os, Loop* loop) {
    const std::set<std::string>& variables = loop->getAllVariables();
    os << "Loop variables: [ ";
    auto vi = variables.begin(), ve = variables.end();
    if (vi == ve) {
      os << "]\n";
    } else {
      for (; vi != ve; ++vi) {
        if (++vi == ve) {
          os << "'" << *--vi << "' ]\n";
        } else {
          os << "'" << *--vi << "', ";
        }
      }
    }
  }

  void Printer::print(std::ostream& os, Loop *loop) {
    os << "==================================================\n";
    os << "Loop name: " << loop->getName() << "\n";
    printVarInvolved(os, loop);
    os << "Loop body:\n";
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      printPath(os, *it);
      os << "\n";
    }
    os << "==================================================\n";
    os.flush();
  }

}
