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
    std::ostringstream oss;
    Printer::printExpression(oss, expr);
    return oss.str();
  }

  void LoopExtraction::handleExpression(SgExpression* expr,
                          VariableTable* varTbl) {
    // depth-first-search, handle all sub-expression first
    if (SgBinaryOp* binOp = dynamic_cast<SgBinaryOp*>(expr)) {
      SgExpression* leftExpr = binOp->get_lhs_operand();
      handleExpression(leftExpr, varTbl);

      SgExpression* rightExpr = binOp->get_rhs_operand();
      handleExpression(rightExpr, varTbl);
    } else if (SgUnaryOp* unaryOp = dynamic_cast<SgUnaryOp*>(expr)) {
      SgExpression* operand = unaryOp->get_operand();
      handleExpression(operand, varTbl);
    }

    // handle all expressions that changes values of variables
    SgUnaryOp *unaryOp;
    if (SgAssignOp *assignOp = dynamic_cast<SgAssignOp*>(expr)) {
      // assignment
      SgExpression* leftExpr = assignOp->get_lhs_operand();
      std::string&& name = getOperandName(leftExpr);
      SgExpression* value = ASTHelper::clone(assignOp->get_rhs_operand());

      Variable* oldVar = varTbl->getVariable(name);
      if (oldVar != nullptr) {
        ASTHelper::replaceVar(value, oldVar->getValue(), name);
      }

      Variable* newVar = new Variable(name, value, varTbl);
      varTbl->addVariable(newVar);
    } else if ((unaryOp = dynamic_cast<SgPlusPlusOp*>(expr))
            || (unaryOp = dynamic_cast<SgMinusMinusOp*>(expr))) {
      // ++/--
      SgExpression* opExpr = unaryOp->get_operand();
      std::string&& name = getOperandName(opExpr);
      SgExpression* value = ASTHelper::clone(unaryOp);

      Variable* oldVar = varTbl->getVariable(name);
      if (oldVar != nullptr) {
        ASTHelper::replaceVar(value, oldVar->getValue(), name);
      }

      Variable* newVar = new Variable(name, value, varTbl);
      varTbl->addVariable(newVar);
    } else if (SgCompoundAssignOp *compoundOp = dynamic_cast<SgCompoundAssignOp*>(expr)) {
      // +=, -=, *= ...
      SgExpression* leftExpr = compoundOp->get_lhs_operand();
      std::string name = getOperandName(leftExpr);

      Variable* oldVar = varTbl->getVariable(name);
      SgExpression *lhsValue, *rhsValue;
      if (oldVar != nullptr) {
        SgCompoundAssignOp* newValue = ASTHelper::clone(compoundOp);
        ASTHelper::replaceVar(newValue, oldVar->getValue(), name);
        lhsValue = newValue->get_lhs_operand();
        rhsValue = newValue->get_rhs_operand();
      } else {
        lhsValue = ASTHelper::clone(leftExpr);
        rhsValue = ASTHelper::clone(compoundOp->get_rhs_operand());
      }

      SgBinaryOp* binOp = ASTHelper::toBinaryOp(
        compoundOp->variantT(), lhsValue, rhsValue, compoundOp->get_type()
      );

      varTbl->addVariable(new Variable(name, binOp, varTbl));
    }
  }

  void LoopExtraction::handleExpression(SgExpression* expr, Loop* loop) {
    // collect variables involved in expression
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

  void LoopExtraction::handleVarDeclaration(SgVariableDeclaration* varDecl, Loop* loop) {

  }

  void LoopExtraction::handleBreakStatement(SgBreakStmt* breakStmt, Loop* loop) {
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      (*it)->setCanBreak(true);
    }
  }

  void LoopExtraction::handleReturnStatement(SgReturnStmt* returnStmt, Loop* loop) {
    // record variable updates in return expression
    SgExpression* expr = returnStmt->get_expression();
    handleExpression(expr, loop);

    // if we encounter a return statement
    // that means all loop path can break the loop
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      (*it)->setCanBreak(true);
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
    handleExpression(condition, loop);

    // fork origin paths and add contradictory constraint
    Loop* newLoop = loop->cloneWithoutBreak();
    for (auto it = newLoop->begin(), ie = newLoop->end(); it != ie; ++it) {
      (*it)->addConstraint(new SgNotOp(condition, condition->get_type()));
    }

    // add constraint to origin loop
    for (auto it = loop->begin(), ie = loop->end(); it != ie; ++it) {
      if (!(*it)->canBreakLoop()) {
        (*it)->addConstraint(condition);
      }
    }

    // handle true body
    SgStatement* trueBody = ifStmt->get_true_body();
    handleStatement(trueBody, loop);


    // handle false body
    SgStatement* falseBody = ifStmt->get_false_body();
    if (falseBody != nullptr) {
      handleStatement(falseBody, newLoop);
    }

    // merge information of false body into true body
    loop->merge(newLoop);
  }

  void LoopExtraction::handleWhileStatment(SgWhileStmt* whileStmt) {

  }

  void LoopExtraction::handleForStatement(SgForStatement* forStmt) {
    VariableTable *varTbl = new VariableTable;
    SgStatement* testStmt = forStmt->get_test();
    SgExprStatement* exprStmt = dynamic_cast<SgExprStatement*>(testStmt);
    SgExpression* condition = nullptr;
    // condition may be null e.g. for(;;) {}
    if (exprStmt != nullptr) {
      condition = ASTHelper::clone(exprStmt->get_expression());
    }

    // create path jumping into loop
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
    handleExpression(condition, varTbl);

    // handle loop body
    SgStatement* bodyStmt = forStmt->get_loop_body();
    handleStatement(bodyStmt, loop);

    // handle increment
    SgExpression* incExpr = forStmt->get_increment();
    handleExpression(incExpr, loop);

    // print result
    Printer::print(std::cout, loop);
  }

  void LoopExtraction::handleDoWhileStatement(SgDoWhileStmt* doStmt) {

  }

  void LoopExtraction::handleStatement(SgStatement* stmt, Loop* loop) {
    if (SgBasicBlock* block = dynamic_cast<SgBasicBlock*>(stmt)) {
      handleLoopBlock(block, loop);
    } else if (SgForStatement* forStmt = dynamic_cast<SgForStatement*>(stmt)) {
      handleForStatement(forStmt);
    } else if (SgWhileStmt* whileStmt = dynamic_cast<SgWhileStmt*>(stmt)) {
      handleWhileStatment(whileStmt);
    } else if (SgDoWhileStmt* doStmt = dynamic_cast<SgDoWhileStmt*>(stmt)) {
      handleDoWhileStatement(doStmt);
    } else if (SgIfStmt *ifStmt = dynamic_cast<SgIfStmt*>(stmt)) {
      handleIfStatement(ifStmt, loop);
    } else if (SgSwitchStatement* switchStmt = dynamic_cast<SgSwitchStatement*>(stmt)) {
      handleSwitchStatement(switchStmt, loop);
    } else if (SgExprStatement *exprStmt = dynamic_cast<SgExprStatement*>(stmt)) {
      SgExpression* expr = exprStmt->get_expression();
      handleExpression(expr, loop);
    } else if (SgVariableDeclaration *varDecl = dynamic_cast<SgVariableDeclaration*>(stmt)) {
      handleVarDeclaration(varDecl, loop);
    } else if (SgBreakStmt* breakStmt = dynamic_cast<SgBreakStmt*>(stmt)) {
      handleBreakStatement(breakStmt,loop);
    } else if (SgReturnStmt *returnStmt = dynamic_cast<SgReturnStmt*>(stmt)) {
      handleReturnStatement(returnStmt, loop);
    } else {
      std::stringstream ss;
      ss << stmt->class_name() << " unsupported in loop block\n";
      Message::warning(ss.str());
    }
  }

  void LoopExtraction::handleLoopBlock(SgBasicBlock* block, Loop* loop) {
    SgStatementPtrList& stmts = block->get_statements();
    for (SgStatement* stmt : stmts) {
      handleStatement(stmt, loop);
    }
  }

  void LoopExtraction::handleSgFunction(SgFunctionDeclaration* func) {
    SgFunctionDefinition* funcDef = func->get_definition();
    SgBasicBlock* funcBody = funcDef->get_body();
    SgStatementPtrList& stmtList = funcBody->get_statements();

    for (SgStatement* stmt : stmtList) {
      if (SgForStatement *forStmt = dynamic_cast<SgForStatement*>(stmt)) {
        handleForStatement(forStmt);
      } else if (SgWhileStmt *whileStmt = dynamic_cast<SgWhileStmt*>(stmt)) {
        handleWhileStatment(whileStmt);
      } else if (SgDoWhileStmt* doStmt = dynamic_cast<SgDoWhileStmt*>(stmt)) {
        handleDoWhileStatement(doStmt);
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
