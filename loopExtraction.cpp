#include "loopExtraction.h"
#include "printer.h"
#include "visitor.h"
#include "astHelper.h"
#include "message.h"
#include "constraint.h"
#include "loopNameAllocator.h"

#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include <set>
#include <map>

namespace LE {

  std::string LoopExtraction::getOperandName(SgExpression* expr) {
    if (SgVarRefExp *varRef = dynamic_cast<SgVarRefExp*>(expr)) {
      return varRef->get_symbol()->get_name().getString();
    } else if (SgPointerDerefExp* ptrDeref = dynamic_cast<SgPointerDerefExp*>(expr)) {
      SgExpression* operand = dynamic_cast<SgExpression*>(ptrDeref->get_operand());
      assert(operand != nullptr);
      std::ostringstream oss;
      oss << "*";
      Printer::printExpression(oss, operand);
      return oss.str();
    } else if (SgPntrArrRefExp* arrRef = dynamic_cast<SgPntrArrRefExp*>(expr)) {
      std::ostringstream oss;
      SgExpression* left = dynamic_cast<SgExpression*>(arrRef->get_lhs_operand());
      assert(left != nullptr);
      Printer::printExpression(oss, left);

      SgExpression* right = dynamic_cast<SgExpression*>(arrRef->get_rhs_operand());
      assert(right != nullptr);
      oss << "[";
      Printer::printExpression(oss, right);
      oss << "]";

      return oss.str();
    } else {
      std::stringstream ss;
      ss << "can not get name from " << expr->class_name() << "\n";
      Message::error(ss.str());
      return "";
    }
  }

  void LoopExtraction::handleExpression(SgExpression* expr,
                          VariableTable* varTbl) {
    // depth-first-search, handle all sub-expression first
    if (SgBinaryOp* binOp = dynamic_cast<SgBinaryOp*>(expr)) {
      SgNode* left = binOp->get_lhs_operand();
      if (SgExpression* leftExpr = dynamic_cast<SgExpression*>(left)) {
        handleExpression(leftExpr, varTbl);
      }

      SgNode* right = binOp->get_rhs_operand();
      if (SgExpression* rightExpr = dynamic_cast<SgExpression*>(right)) {
        handleExpression(rightExpr, varTbl);
      }
    } else if (SgUnaryOp* unaryOp = dynamic_cast<SgUnaryOp*>(expr)) {
      SgNode* operand = unaryOp->get_operand();
      if (SgExpression* e = dynamic_cast<SgExpression*>(operand)) {
        handleExpression(e, varTbl);
      }
    }

    // handle all expressions that changes values of variables
    SgUnaryOp *unaryOp;
    if (SgAssignOp *assignOp = dynamic_cast<SgAssignOp*>(expr)) {
      SgExpression* leftExpr = assignOp->get_lhs_operand();
      std::string name = getOperandName(leftExpr);
      SgExpression* value = ASTHelper::clone(assignOp->get_rhs_operand());

      Variable* oldVar = varTbl->getVariable(name);
      if (oldVar != nullptr) {
        ASTHelper::replaceVar(value, oldVar->getValue(), name);
      }

      Variable* newVar = new Variable(name, value, varTbl);
      varTbl->addVariable(newVar);
    } else if ((unaryOp = dynamic_cast<SgPlusPlusOp*>(expr))
            || (unaryOp = dynamic_cast<SgMinusMinusOp*>(expr))) {
      SgExpression* opExpr = unaryOp->get_operand();
      std::string name = getOperandName(opExpr);
      SgExpression* value = ASTHelper::clone(unaryOp);

      Variable* oldVar = varTbl->getVariable(name);
      if (oldVar != nullptr) {
        ASTHelper::replaceVar(value, oldVar->getValue(), name);
      }

      Variable* newVar = new Variable(name, value, varTbl);
      varTbl->addVariable(newVar);
    } else if (SgCompoundAssignOp *compoundOp = dynamic_cast<SgCompoundAssignOp*>(expr)) {
      SgExpression* leftExpr = compoundOp->get_lhs_operand();
      std::string name = getOperandName(leftExpr);

      Variable* oldVar = varTbl->getVariable(name);
      Variable* newVar;
      if (oldVar != nullptr) {
        SgCompoundAssignOp* newValue = ASTHelper::clone(compoundOp);
        ASTHelper::replaceVar(newValue, oldVar->getValue(), name);
        newVar = new Variable(name, ASTHelper::toBinaryOp(
          compoundOp->variantT(), newValue->get_lhs_operand(),
          newValue->get_rhs_operand(), compoundOp->get_type()
        ), varTbl);
      } else {
        SgExpression* value = ASTHelper::clone(compoundOp->get_rhs_operand());
        newVar = new Variable(name, ASTHelper::toBinaryOp(
          compoundOp->variantT(), leftExpr, value, compoundOp->get_type()
        ), varTbl);
      }

      varTbl->addVariable(newVar);
    }
  }

  void LoopExtraction::handleCondition(SgExpression* condition, Loop* loop) {
    VariableCollector collector(loop);
    collector.traverse(condition, preorder);

    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      if (!(*it)->canBreakLoop()) {
        VariableTable* tbl = (*it)->getVariableTable();
        handleExpression(condition, tbl);
      }
    }
  }

  void LoopExtraction::handleLoopBlock(SgStatement* block, Loop* loop) {
    std::vector<SgNode*> stmts = block->get_traversalSuccessorContainer();
    for (SgNode* stmt : stmts) {
      if (SgIfStmt *ifStmt = dynamic_cast<SgIfStmt*>(stmt)) {
        handleIfStatement(ifStmt, loop);
      } else if (SgForStatement* forStmt = dynamic_cast<SgForStatement*>(stmt)) {
        handleForStatement(forStmt);
      } else if (SgWhileStmt* whileStmt = dynamic_cast<SgWhileStmt*>(stmt)) {
        handleWhileStatment(whileStmt);
      }else if (SgSwitchStatement* switchStmt = dynamic_cast<SgSwitchStatement*>(stmt)) {
        handleSwitchStatement(switchStmt, loop);
      } else if (SgExprStatement *exprStmt = dynamic_cast<SgExprStatement*>(stmt)) {
        handleExprStatement(exprStmt, loop);
      } else if (SgBreakStmt* breakStmt = dynamic_cast<SgBreakStmt*>(stmt)) {
        handleBreakStatement(breakStmt,loop);
      } else if (SgReturnStmt *returnStmt = dynamic_cast<SgReturnStmt*>(stmt)) {
        handleReturnStatement(returnStmt, loop);
      } else {
        std::stringstream ss;
        ss << stmt->class_name() << " unsupported now\n";
        Message::warning(ss.str());
      }
    }
  }

  void LoopExtraction::handleExprStatement(SgExprStatement* exprStmt, Loop* loop) {
    SgExpression* expr = exprStmt->get_expression();
    VariableCollector collector(loop);
    collector.traverse(expr, preorder);

    // for each loop path, update variable table
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      // if the path can break the loop, ignore it
      if (!(*it)->canBreakLoop()) {
        VariableTable *tbl = (*it)->getVariableTable();
        handleExpression(expr, tbl);
      }
    }
  }

  void LoopExtraction::handleBreakStatement(SgBreakStmt* breakStmt, Loop* loop) {
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      (*it)->setCanBreak(true);
    }
  }

  void LoopExtraction::handleReturnStatement(SgReturnStmt* returnStmt, Loop* loop) {
    // if we encounter a return statement
    // that means all loop path can break the loop
    SgExpression* expr = returnStmt->get_expression();
    VariableCollector collector(loop);
    collector.traverse(expr, preorder);

    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      if (!(*it)->canBreakLoop()) {
        VariableTable* tbl = (*it)->getVariableTable();
        handleExpression(expr, tbl);
        (*it)->setCanBreak(true);
      }
    }
  }

  void LoopExtraction::handleSwitchStatement(SgSwitchStatement* switchStmt,
                                             Loop* loop) {

  }

  void LoopExtraction::handleIfStatement(SgIfStmt* ifStmt, Loop* loop) {
    // handle condition
    SgStatement* conditionStmt = ifStmt->get_conditional();
    SgExprStatement* exprStmt = dynamic_cast<SgExprStatement*>(conditionStmt);
    assert(exprStmt != nullptr);
    SgExpression* condition = exprStmt->get_expression();
    handleCondition(condition, loop);

    // clone origin paths and fork new ones with contradictory constraint
    Loop* newLoop = loop->cloneWithoutBreak();
    for (auto it = newLoop->begin(), ie = newLoop->end(); it != ie; ++it) {
      (*it)->addConstraint(new SgNotOp(condition, condition->get_type()));
    }

    // add constraint for origin loop
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      if (!(*it)->canBreakLoop()) {
        (*it)->addConstraint(condition);
      }
    }

    // handle true body
    SgStatement* trueBody = ifStmt->get_true_body();
    if (SgExprStatement* bodyExpr = dynamic_cast<SgExprStatement*>(trueBody)) {
      handleExprStatement(bodyExpr, loop);
    } else {
      handleLoopBlock(trueBody, loop);
    }

    // handle false body
    SgStatement* falseBody = ifStmt->get_false_body();
    if (falseBody != nullptr) {
      if (SgExprStatement* bodyExpr = dynamic_cast<SgExprStatement*>(trueBody)) {
        handleExprStatement(bodyExpr, newLoop);
      } else {
        handleLoopBlock(falseBody, newLoop);
      }
    }

    loop->merge(newLoop);
  }

  void LoopExtraction::handleWhileStatment(SgWhileStmt* whileStmt) {
    std::cout << "while" << std::endl;
  }

  void LoopExtraction::handleForStatement(SgForStatement* forStmt) {
    // collect variables involved in for_init_statement
    // and store them in VariableTable
    VariableTable *varTbl = new VariableTable;
    SgStatement* testStmt = forStmt->get_test();
    SgExprStatement* exprStmt = dynamic_cast<SgExprStatement*>(testStmt);
    SgExpression* condition = nullptr;
    // condition may be null e.g. for(;;) {}
    if (exprStmt != nullptr) {
      condition = ASTHelper::clone(exprStmt->get_expression());
    }

    // create path jumping in loop
    ConstraintList* inConstraint = new ConstraintList;
    if (condition != nullptr) {
      inConstraint->addConstraint(condition);
    } else {
      inConstraint->addConstraint(new SgBoolValExp(1));
    }
    LoopPath* inPath = new LoopPath(varTbl, inConstraint, false);

    // create path jumping out of loop
    ConstraintList* outConstraint = new ConstraintList;
    if (condition != nullptr) {
      outConstraint->addConstraint(new SgNotOp(condition, condition->get_type()));
    } else {
      outConstraint->addConstraint(new SgBoolValExp(0));
    }
    LoopPath* outPath = new LoopPath(varTbl->clone(), outConstraint, true);

    // create loop
    Loop *loop = new Loop(LoopNameAllocator::allocLoopName());
    loop->addPath(inPath);
    loop->addPath(outPath);

    // collect variables involved in for_test_statement
    // and store them in VariableTable
    handleCondition(condition, loop);

    // handle loop body
    SgStatement* bodyStmt = forStmt->get_loop_body();
    handleLoopBlock(bodyStmt, loop);

    // hanlde increment
    SgExpression* incExpr = forStmt->get_increment();
    VariableCollector collector(loop);
    collector.traverse(incExpr, preorder);
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      if (!(*it)->canBreakLoop()) {
        handleExpression(incExpr, (*it)->getVariableTable());
      }
    }

    Printer::print(std::cout, loop);
  }

  void LoopExtraction::handleSgFunction(SgFunctionDeclaration* func) {
    SgFunctionDefinition* funcDef = func->get_definition();
    SgBasicBlock* funcBody = funcDef->get_body();
    SgStatementPtrList& stmtList = funcBody->get_statements();

    for (auto it = stmtList.begin(), ie = stmtList.end(); it != ie; ++it) {
      if (SgForStatement *forStmt = dynamic_cast<SgForStatement*>(*it)) {
        handleForStatement(forStmt);
      } else if (SgWhileStmt *whileStmt = dynamic_cast<SgWhileStmt*>(*it)) {
        handleWhileStatment(whileStmt);
      }
    }
  }

  void LoopExtraction::handleSgGlobal(SgGlobal* global) {
    std::vector<SgNode*> decls = global->get_traversalSuccessorContainer();

    // the first four declarations are not user-deifined, so we skip it
    // this conclusion is attained from observation
    int size = decls.size();
    for (int i = 5; i < size; ++i) {
      SgNode *decl = decls[i];
      if (SgFunctionDeclaration *func = dynamic_cast<SgFunctionDeclaration*>(decl)) {
        handleSgFunction(func);
      }
    }
  }

  void LoopExtraction::handleSgSourceFile(SgSourceFile* src) {
    SgGlobal* global = src->get_globalScope();
    handleSgGlobal(global);
  }

  void LoopExtraction::handleSgFileList(SgFileList* fileList) {
    std::vector<SgNode*> nodes = fileList->get_traversalSuccessorContainer();
    for (auto n : nodes) {
      if (SgSourceFile *src = dynamic_cast<SgSourceFile*>(n)) {
        handleSgSourceFile(src);
      }
    }
  }

  void LoopExtraction::handleSgProject() {
    SgFileList* fileList = project->get_fileList_ptr();
    handleSgFileList(fileList);
  }

  LoopExtraction::LoopExtraction (int argc, char* argv[]) {
    project = frontend(argc, argv);
  }

}
