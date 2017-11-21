#ifndef LOOP_EXTRACTION_H
#define LOOP_EXTRACTION_H

#include "rose.h"
#include "loop.h"

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
    void handleExpression(SgExpression* expr, Loop* loop);

    // record variable updates in declaration
    void handleVarDeclaration(SgVariableDeclaration* varDecl, Loop* loop);

    // mark current paths canBreak = true
    void handleBreakStatement(SgBreakStmt* breakStmt, Loop* loop);

    // mark current paths canBreak = true
    // record variable updates in returned expression
    void handleReturnStatement(SgReturnStmt* returnStmt, Loop* loop);

    // forks loop paths
    // record variable updates in each case
    void handleSwitchStatement(SgSwitchStatement* switchStmt, Loop* loop);

    // forks loop paths
    // record variable updates in condition, true body and false body
    // e.g. if (a = 3)
    void handleIfStatement(SgIfStmt* ifStmt, Loop* loop);

    // extract a loop from a while statement
    void handleWhileStatment(SgWhileStmt* whileStmt, Loop* loop);

    // extract a loop from a for statement
    void handleForStatement(SgForStatement* forStmt, Loop* loop);

    // extract a loop from a do-while statement
    void handleDoWhileStatement(SgDoWhileStmt* doStmt, Loop* loop);

    // handle a general statement
    // it delegates jobs to other concrete statements
    void handleStatement(SgStatement* stmt, Loop* loop);

    // traverse statements in a block of a loop
    void handleLoopBlock(SgBasicBlock* block, Loop* loop);

    // extract loops from a function
    void handleSgFunction(SgFunctionDeclaration* func);

    // extract loops from a list of global functions
    void handleSgGlobal(SgGlobal* global);

    // extract loops from a source file
    void handleSgSourceFile(SgSourceFile* src);

    // extract loops from a list of files
    void handleSgFileList(SgFileList* fileList);

  public:
    LoopExtraction (int argc, char* argv[]);

    // extract loops from a project
    void handleSgProject();
  };

}

#endif
