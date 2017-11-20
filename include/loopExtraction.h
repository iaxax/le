#ifndef LOOP_EXTRACTION_H
#define LOOP_EXTRACTION_H

#include "rose.h"
#include "variable.h"
#include "loop.h"

namespace LE {

  class LoopExtraction {
  private:
    SgProject *project;

    // so far, there are three types of operand that has name
    // a variable, a pointer or an array
    // return name of operand
    std::string getOperandName(SgExpression* expr);

    // update values of all variables in variable table
    void handleExpression(SgExpression* expr, VariableTable* varTbl);

    // update values of all variables in all paths
    void handleCondition(SgExpression* condition, Loop* loop);

    // handle statement block in a loop
    void handleLoopBlock(SgStatement* block, Loop* loop);

    void handleExprStatement(SgExprStatement* exprStmt, Loop* loop);

    void handleBreakStatement(SgBreakStmt* breakStmt, Loop* loop);

    void handleReturnStatement(SgReturnStmt* returnStmt, Loop* loop);

    void handleSwitchStatement(SgSwitchStatement* switchStmt, Loop* loop);

    void handleIfStatement(SgIfStmt* ifStmt, Loop* loop);

    void handleWhileStatment(SgWhileStmt* whileStmt);

    void handleForStatement(SgForStatement* forStmt);

    void handleSgFunction(SgFunctionDeclaration* func);

    void handleSgGlobal(SgGlobal* global);

    void handleSgSourceFile(SgSourceFile* src);

    void handleSgFileList(SgFileList* fileList);

  public:
    LoopExtraction (int argc, char* argv[]);
    void handleSgProject();
  };

}

#endif
