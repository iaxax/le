#ifndef LOOP_EXTRACTION_PRINTER_H
#define LOOP_EXTRACTION_PRINTER_H

#include "program.h"
#include <ostream>
#include <string>
#include <set>

namespace LE {

  class Printer {
  public:
    virtual void printProgram(std::ostream& os, Program* program, int indentLv = 0) = 0;
    virtual void printFunctions(std::ostream& os, const std::set<Function*>& functions, int indentLv = 0) = 0;
    virtual void printPaths(std::ostream& os, const std::set<Path*>& paths, int indentLv = 0) = 0;
    virtual void printLoops(std::ostream& os, const std::set<Loop*>& loops, int indentLv = 0) = 0;
    virtual void printLoopPaths(std::ostream& os, const std::set<LoopPath*>& loopPath, int indentLv = 0) = 0;
    virtual void printExpression(std::ostream& os, SgExpression* expr);

    Printer(): indent("    ") {}
    virtual ~Printer() {};

  protected:
    std::string indent;

    std::string getIndent(int level);
  };

  // // print result of loop extraction
  // class NormalPrinter: public Printer {
  // private:
  //   // indentation for pretty printing
  //   const std::string indent;
  //
  //   // print update of variables in a path
  //   // see README for more detail
  //   void printVariableUpdate(std::ostream& os, VariableTable* varTbl);
  //
  //   // print constraints of a path
  //   // see README for more detail
  //   void printConstraint(std::ostream& os, ConstraintList* cl);
  //
  //   // print variables involved in a loop
  //   // see README for more detail
  //   void printVarInvolved(std::ostream& os, const std::set<std::string>& variables);
  //
  //   // print names of inner loops
  //   void printInnerLoops(std::ostream& os, const std::set<std::string>& loopNames);
  //
  // public:
  //   virtual void printProgram(std::ostream& os, Program* program, int indentLv = 0);
  //   virtual void printFunctions(std::ostream& os, const std::set<Function*>& functions, int indentLv = 0);
  //   virtual void printPaths(std::ostream& os, const std::set<Path*>& paths, int indentLv = 0);
  //   virtual void printLoops(std::ostream& os, const std::set<Loop*>& loops, int indentLv = 0);
  //   virtual void printLoopPaths(std::ostream& os, const std::set<LoopPath*>& loopPath, int indentLv = 0);
  //   virtual void printExpression(std::ostream& os, SgExpression* expr);
  //
  //   NormalPrinter() {}
  //   NormalPrinter(const std::string& indent): indent(indent) {}
  //   virtual ~NormalPrinter() {}
  // };

  class JsonPrinter: public Printer {
  private:
    void printVarDecl(std::ostream& os, VariableTable* varTbl, int indentLv);
    void printVarInit(std::ostream& os, VariableTable* varTbl, int indentLv);
    void printVariable(std::ostream& os, Variable* var, int indentLv);
    void printCanBreak(std::ostream& os, bool canBreak, int indentLv);
    void printConstraints(std::ostream& os, ConstraintList* cl, int indentLv);
    void printFuncParam(std::ostream& os, const std::set<std::string>& params, int indentLv);
    void printReturnValue(std::ostream& os, SgExpression* retVal, int indentLv);
    void printPath(std::ostream& os, Path* path, int indentLv);
    void printPaths(std::ostream& os, const std::vector<std::string>& p, int indentLv);
    void printBlock(std::ostream& os, Block* block, int indentLv);
    void printBlocks(std::ostream& os, const std::set<Block*>& blocks, int indentLv);
    void printLoopPath(std::ostream& os, LoopPath* loopPath, int indentLv);
    void printLoopPath(std::ostream& os, const std::set<std::string>& innerLoops, VariableTable* varTbl, int indentLv);
    void printLoop(std::ostream& os, Loop* loop, std::set<Loop*>& printed, int indentLv);
    void printInnerLoops(std::ostream& os, const std::set<Loop*>& loops, std::set<Loop*>& printed, int indentLv);
    void printFunction(std::ostream& os, Function* func, int indentLv);
    void printProgramName(std::ostream& os, const std::string& name, int indentLv);

  public:
    virtual void printProgram(std::ostream& os, Program* program, int indentLv = 0);
    virtual void printFunctions(std::ostream& os, const std::set<Function*>& functions, int indentLv = 0);
    virtual void printPaths(std::ostream& os, const std::set<Path*>& paths, int indentLv = 0);
    virtual void printLoops(std::ostream& os, const std::set<Loop*>& loops, int indentLv = 0);
    virtual void printLoopPaths(std::ostream& os, const std::set<LoopPath*>& paths, int indentLv = 0);
    virtual void printExpression(std::ostream& os, SgExpression* expr);

    virtual ~JsonPrinter() {}
  };
}

#endif
