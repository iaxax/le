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
    if (expr == nullptr) {
      os << "[NULL]";
      return;
    }

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

  //
  // void NormalPrinter::printInnerLoops(std::ostream& os, const std::set<std::string>& loopNames) {
  //   os << indent << "inner loop: {";
  //   if (loopNames.empty()) {
  //     os << "}\n";
  //     return;
  //   }
  //
  //   for (auto it = loopNames.begin(), ie = loopNames.end(); it != ie; ++it) {
  //     if (++it == ie) {
  //       os << " [" << *--it << "] }\n";
  //     } else {
  //       os << " [" << *--it << "],";
  //     }
  //   }
  // }
  //
  // void NormalPrinter::printLoopPath(std::ostream& os, LoopPath* path, int indentLv) {

  // }
  //

  // void NormalPrinter::printFunctions(std::ostream& os, const std::set<Function*>& functions, int indentLv) {
  //
  // }
  //
  // void NormalPrinter::printProgram(std::ostream& os, Program* program, int indentLv) {
  //
  // }

  void JsonPrinter::printExpression(std::ostream& os, SgExpression* expr) {
    Printer::printExpression(os, expr);
  }

  void JsonPrinter::printConstraints(std::ostream& os, ConstraintList* cl, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'constraints': '";
    auto it = cl->begin(), ie = cl->end();
    if (it == ie) {
      os << "true'";
    }
    else {
      while (it != ie) {
        SgExpression* constraint = *(it++);
        printExpression(os, constraint);
        os << (it == ie ? "'" : " && ");
      }
    }
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

        std::ostringstream oss;
        printExpression(oss, initValue);
        value = oss.str();

        os << " '" << name << "': '" << value << "'";
        os << (it == ie ? " }" : ",");
      }
    }
  }

  void JsonPrinter::printVariable(std::ostream& os, Variable* var, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'" << var->getName() << "': '";
    printExpression(os, var->getValue());
    os << "'";
  }

  void JsonPrinter::printCanBreak(std::ostream& os, bool canBreak, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'break': " << (canBreak ? "'true'" : "'false'");
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

  void JsonPrinter::printReturnValue(std::ostream& os, SgExpression* retVal, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'return': '";
    printExpression(os, retVal);
    os << "'";
  }

  void JsonPrinter::printLoopPath(std::ostream& os, const std::set<std::string>& innerLoops,
                                  VariableTable* varTbl, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'path': [";
    auto it = varTbl->begin(), ie = varTbl->end();
    while (it != ie) {
      Variable* var = (it++)->second;
      printVariable(os, var, 0);
      if (it != ie) os << ", ";
    }

    auto lit = innerLoops.begin(), lie = innerLoops.end();
    if (lit == lie) {
      os << "]";
    } else {
      os << ", ";
      while (lit != lie) {
        const std::string& str = *(lit++);
        os << "'{" << str << "}'";
        os << (lit == lie ? "]" : ", ");
      }
    }

  }

  void JsonPrinter::printLoopPath(std::ostream& os, LoopPath* loopPath, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "{\n";

    printConstraints(os, loopPath->getConstraintList(), indentLv + 1);
    os << ",\n";

    printLoopPath(os, loopPath->getInnerLoops(), loopPath->getVariableTable(), indentLv + 1);
    os << ",\n";

    printCanBreak(os, loopPath->canBreakLoop(), indentLv + 1);
    os << '\n';

    os << indent << "}";
  }

  void JsonPrinter::printLoopPaths(std::ostream& os, const std::set<LoopPath*>& paths, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'paths': [";
    auto it = paths.begin(), ie = paths.end();
    if (it == ie) {
      os << ']';
    } else {
      os << '\n';
      while (it != ie) {
        LoopPath* loopPath = *(it++);
        printLoopPath(os, loopPath, indentLv + 1);
        os << (it == ie ? "\n" : ",\n");
      }
      os << indent << "]";
    }
  }

  void JsonPrinter::printLoop(std::ostream& os, Loop* loop, std::set<Loop*>& printed, int indentLv) {
    if (printed.find(loop) != printed.end()) return;

    printed.insert(loop);
    std::string&& indent = getIndent(indentLv);
    os << indent << "'" << loop->getName() << "': {\n";

    printVarDecl(os, loop->getVariableTable(), indentLv + 1);
    os << ",\n";

    printVarInit(os, loop->getVariableTable(), indentLv + 1);
    os << ",\n";

    printLoopPaths(os, loop->getPaths(), indentLv + 1);
    os << '\n';

    os << indent << "}";

    const std::set<Loop*>& innerLoops = loop->getInnerLoops();
    if (!innerLoops.empty()) {
      os << ",\n";
      printInnerLoops(os, innerLoops, printed, indentLv);
    }
  }

  void JsonPrinter::printInnerLoops(std::ostream& os, const std::set<Loop*>& loops,
                                    std::set<Loop*>& printed, int indentLv) {
    auto it = loops.begin(), ie = loops.end();
    while (it != ie) {
      Loop* loop = *(it++);
      printLoop(os, loop, printed, indentLv);
      if (it != ie) os << ",\n";
    }
  }

  void JsonPrinter::printLoops(std::ostream& os, const std::set<Loop*>& loops, int indentLv) {
      std::string&& indent = getIndent(indentLv);
      os << indent << "'loops': {";

      auto it = loops.begin(), ie = loops.end();
      if (it == ie) {
        os << '}';
      } else {
        os << '\n';
        std::set<Loop*>* printed = new std::set<Loop*>;
        while (it != ie) {
          Loop* loop = *(it++);
          printLoop(os, loop, *printed, indentLv + 1);
          os << (it == ie ? "\n" : ",\n");
        }
        os << indent << "}";
        delete printed;
      }

  }

  void JsonPrinter::printPaths(std::ostream& os, const std::vector<std::string>& p, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'path': [";
    auto it = p.begin(), ie = p.end();
    if (it == ie) {
      os << "]";
    } else {
      os << ' ';
      while (it != ie) {
        const std::string& str = *(it++);
        os << "'{" << str << "}'" << (it == ie ? " ]" : ", ");
      }
    }
  }

  void JsonPrinter::printPath(std::ostream& os, Path* path, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'" + path->getName() << "': {\n";

    printConstraints(os, path->getConstraints(), indentLv + 1);
    os << ",\n";

    printPaths(os, path->getPaths(), indentLv + 1);
    os << ",\n";

    printReturnValue(os, path->getReturnValue(), indentLv + 1);
    os << '\n';

    os << indent << "}";
  }

  void JsonPrinter::printPaths(std::ostream& os, const std::set<Path*>& paths, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'paths': {";
    auto it = paths.begin(), ie = paths.end();
    if (it == ie) {
      os << "}";
    } else {
      os << '\n';
      while (it != ie) {
        Path* path = *(it++);
        printPath(os, path, indentLv + 1);
        os << (it == ie ? "\n" : ",\n");
      }
      os << indent << "}";
    }
  }

  void JsonPrinter::printBlock(std::ostream& os, Block* block, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'" << block->getName() << "': {";

    VariableTable* varTbl = block->getVariableTable();
    auto it = varTbl->begin(), ie = varTbl->end();
    if (it == ie) {
      os << "}";
    } else {
      os << '\n';
      while (it != ie) {
        Variable* var = (it++)->second;
        printVariable(os, var, indentLv + 1);
        os << (it == ie ? "\n" : ",\n");
      }
      os << indent << "}";
    }
  }

  void JsonPrinter::printBlocks(std::ostream& os, const std::set<Block*>& blocks, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'blocks': {";
    auto it = blocks.begin(), ie = blocks.end();
    if (it == ie) {
      os << '}';
    } else {
      os << '\n';
      while (it != ie) {
        Block* block = *(it++);
        printBlock(os, block, indentLv + 1);
        os << (it == ie ? "\n" : ",\n");
      }
      os << indent << "}";
    }
  }

  void JsonPrinter::printFunction(std::ostream& os, Function* func, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'" << func->getName() << "': {\n";

    printVarDecl(os, func->getVariableTable(), indentLv + 1);
    os << ",\n";

    printVarInit(os, func->getVariableTable(), indentLv + 1);
    os << ",\n";

    printFuncParam(os, func->getParams(), indentLv + 1);
    os << ",\n";

    printPaths(os, func->getPaths(), indentLv + 1);
    os << ",\n";

    printBlocks(os, func->getBlocks(), indentLv + 1);
    os << ",\n";

    printLoops(os, func->getLoops(), indentLv + 1);
    os << '\n';

    os << indent << "}";
  }

  void JsonPrinter::printFunctions(std::ostream& os, const std::set<Function*>& funcs, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'functions': {";
    auto it = funcs.begin(), ie = funcs.end();
    if (it == ie) {
      os << "}";
    } else {
      os << '\n';
      while (it != ie) {
        Function* func = *(it++);
        printFunction(os, func, indentLv + 1);
        os << (it == ie ? "\n" : ",\n");
      }
      os << indent << "}";
    }
  }

  void JsonPrinter::printProgramName(std::ostream& os, const std::string& name, int indentLv) {
    std::string&& indent = getIndent(indentLv);
    os << indent << "'program_name': '" << name << "'";
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

    printFunctions(os, program->getFunctions(), indentLv + 1);
    os << "\n";

    os << indent << "}\n";
    os.flush();
  }
}
