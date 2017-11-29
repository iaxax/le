#include "printer.h"
#include "astHelper.h"
#include "message.h"
#include <cassert>
#include <set>
#include <sstream>

namespace LE {

  std::string Printer::getIndent(int level) {
    std::stringstream ss;
    for (int i = 0; i < level; ++i) {
      ss << indent;
    }
    return ss.str();
  }

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

  void NormalPrinter::printVariableUpdate(std::ostream& os, VariableTable* varTbl) {
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

  void NormalPrinter::printConstraint(std::ostream& os, ConstraintList* cl) {
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

  void NormalPrinter::printInnerLoops(std::ostream& os, const std::set<std::string>& loopNames) {
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

  void NormalPrinter::printLoopPath(std::ostream& os, LoopPath* path, int indentLv) {
    printVariableUpdate(os, path->getVariableTable());
    printInnerLoops(os, path->getInnerLoops());
    printConstraint(os, path->getConstraintList());

    os << indent << "break: ";
    os << (path->canBreakLoop() ? "true" : "false") << "\n";
  }

  void NormalPrinter::printVarInvolved(std::ostream& os,
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

  void NormalPrinter::printLoop(std::ostream& os, Loop *loop, int indentLv) {
    os << "==================================================\n";
    // print name
    os << "Loop name: " << loop->getName() << "\n";

    // print variables
    const std::set<std::string>& variables = loop->getAllVariables();
    printVarInvolved(os, variables);

    // print body
    os << "Loop body:\n";
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      printLoopPath(os, *it);
      os << "\n";
    }

    os << "==================================================\n";
    os.flush();
  }

  void NormalPrinter::printFunction(std::ostream& os, Function* function, int indentLv) {

  }

  void NormalPrinter::printProgram(std::ostream& os, Program* program, int indentLv) {

  }

  void JsonPrinter::printVarDecl(std::ostream& os, VariableTable* varTbl, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'variables': {";
    auto it = varTbl->begin(), ie = varTbl->end();
    if (it == ie) {
      os << "}";
    } else {
      while (it != ie) {
        Variable* var = (it++)->second;
        std::string&& type = ASTHelper::getTypeString(var->getType());
        os << " '" << var->getName() << "': '" << type << "'";
        os << (it == ie ? " }" : ",");
      }
    }
  }

  void JsonPrinter::printVarInit(std::ostream& os, VariableTable* varTbl, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'initialize': {";
    auto it = varTbl->begin(), ie = varTbl->end();
    if (it == ie) {
      os << "}";
    } else {
      while (it != ie) {
        Variable* var = (it++)->second;
        std::string&& name = var->getName();
        SgExpression* initValue = var->getInitValue();
        std::string value;
        if (initValue == nullptr) {
          value = "[NULL]";
        } else {
          std::ostringstream oss;
          printExpression(oss, initValue);
          value = oss.str();
        }

        os << " '" << name << "': '" << value << "'";
        os << (it == ie ? " }" : ",");
      }
    }
  }

  void JsonPrinter::printFuncParam(std::ostream& os,
                                  const std::set<std::string>& params,
                                  int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'input_variables': [";
    auto it = params.begin(), ie = params.end();
    if (it == ie) {
      os << "]";
    } else {
      while (it != ie) {
        const std::string& name = *(it++);
        os << " '" << name << "'";
        os << (it == ie ? " ]" : ",");
      }
    }
  }

  void JsonPrinter::printLoopPath(std::ostream& os, LoopPath* LoopPath, int indentLv) {

  }

  void JsonPrinter::printLoop(std::ostream& os, Loop* loop, int indentLv) {

  }

  void JsonPrinter::printFunction(std::ostream& os, Function* function, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'" << function->getName() << "': {\n";

    printVarDecl(os, function->getVariableTable(), indentLv + 1);
    os << ",\n";

    printFuncParam(os, function->getParams(), indentLv + 1);
    os << ",\n";

    os << indent << "}";
  }

  void JsonPrinter::printProgramName(std::ostream& os, const std::string& name, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'program_name': '" << name << "'\n";
  }

  void JsonPrinter::printProgram(std::ostream& os, Program* program, int indentLv) {
    std::string&& indent = getIndent(indentLv);

    os << indent << "{\n";

    printProgramName(os, program->getName(), indentLv + 1);
    os << ",\n";

    printVarDecl(os, program->getVariableTable(), indentLv + 1);
    os << ",\n";
    printVarInit(os, program->getVariableTable(), indentLv + 1);
    os << ",\n";

    std::string&& deeperIndent = getIndent(indentLv + 1);
    os << deeperIndent << "'functions': {";
    const std::set<Function*> funcs = program->getFunctions();
    auto it = funcs.begin(), ie = funcs.end();
    if (it == ie) {
      os << " }\n";
    } else {
      os << '\n';
      while (it != ie) {
        Function* func = *(it++);
        printFunction(os, func, indentLv + 2);
        os << (it == ie ? "\n" : ",\n");
      }
      os << deeperIndent << "}\n";
    }

    os << indent << "}\n";
  }
}
