#ifndef LOOP_EXTRACTION_H
#define LOOP_EXTRACTION_H

#include "rose.h"
#include "program.h"

namespace LE {

  // do loop Extraction
  // a project forms a tree structure
  // like recursive descent in complier
  // I define a function for a kind of node in project tree
  class LoopExtraction {
  private:
    SgProject *project;

    // so far, there are three types of operands that have name
    // a variable, a pointer or an array
    // return name of operand
    // e.g 'var', 'arr[i]', '*p'
    std::string getOperandName(SgExpression* expr);

    // add loop to paths and function and return the loop
    Loop* saveLoopInFunction(Function* func);

    // for, while, do-while, if, and switch-case are atom statements
    // others are not
    bool isAtomStatement(SgStatement* stmt);

    // if we can't find a variable in current variable table
    // it may locate in variable table of a outer loop
    // so we search along this link and find the nearest variable table
    // and update the value of the variable
    // if we find this variable in outer loop, return true, otherwise false
    // bool updateNearestVar(SgExpression* value, const std::string &name,
    //                       VariableTable* varTbl);

    // record variable updates in an expression
    void handleExpression(SgExpression* expr, VariableTable* varTbl);

    // record variable updates in expression
    void handleExprInLoop(SgExpression* expr, Loop* loop);

    // record variable updates in declaration
    void handleVarDeclaration(SgVariableDeclaration* varDecl,
                              VariableTable* varTbl);

    // mark current paths canBreak = true
    void handleBreakInLoop(SgBreakStmt* breakStmt, Loop* loop);

    // mark current paths canBreak = true
    // record variable updates in returned expression
    void handleReturnInLoop(SgReturnStmt* returnStmt, Loop* loop);

    // forks loop paths
    // record variable updates in each case
    void handleSwitchStatement(SgSwitchStatement* switchStmt, Loop* loop);

    // forks loop paths
    // record variable updates in condition, true body and false body
    // e.g. if (a = 3)
    void handeIfInLoop(SgIfStmt* ifStmt, Loop* loop);

    // record paths in if-statement
    void handleIfInFunction(SgIfStmt* ifStmt, Function* func);

    // extract a loop from a while statement
    void handleWhileStatment(SgWhileStmt* whileStmt, Loop* loop);

    // extract a loop from a for statement
    void handleForStatement(SgForStatement* forStmt, Loop* loop);

    // extract a loop from a do-while statement
    void handleDoWhileStatement(SgDoWhileStmt* doStmt, Loop* loop);

    // handle a general statement in a loop
    // it delegates jobs to other concrete statements
    void handleStmtInLoop(SgStatement* stmt, Loop* loop);

    // handle a general statement in a function
    // it delegates jobs to other concrete statements
    void handleStmtInFunction(SgStatement* stmt, Function* func);

    // traverse statements in a block of a loop
    void handleBlockInLoop(SgStatementPtrList& stmts, Loop* loop);

    // traverse statements in a block of a path
    void handleBlockInFunction(SgStatementPtrList& stmts, Function* func);

    // extract paths from a function
    void handleSgFunction(SgFunctionDeclaration* funcDecl, Function* func);

    // extract paths from a list of global functions
    void handleSgGlobal(SgGlobal* global, Program* program);

    // extract paths from a source file
    void handleSgSourceFile(SgSourceFile* src);

    // extract paths from a list of files
    void handleSgFileList(SgFileList* fileList);

  public:
    LoopExtraction (int argc, char* argv[]);

    // extract paths from a project
    void handleSgProject();
  };

}

#endif
