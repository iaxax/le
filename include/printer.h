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
    virtual void printFunction(std::ostream& os, Function* function, int indentLv = 0) = 0;
    virtual void printLoop(std::ostream& os, Loop* loop, int indentLv = 0) = 0;
    virtual void printLoopPath(std::ostream& os, LoopPath* loopPath, int indentLv = 0) = 0;
    virtual void printExpression(std::ostream& os, SgExpression* expr);

    Printer(): indent("    ") {}
    virtual ~Printer() {};

  protected:
    std::string indent;

    std::string getIndent(int level);
  };

  // print result of loop extraction
  class NormalPrinter: public Printer {
  private:
    // indentation for pretty printing
    const std::string indent;

    // print update of variables in a path
    // see README for more detail
    void printVariableUpdate(std::ostream& os, VariableTable* varTbl);

    // print constraints of a path
    // see README for more detail
    void printConstraint(std::ostream& os, ConstraintList* cl);

    // print variables involved in a loop
    // see README for more detail
    void printVarInvolved(std::ostream& os, const std::set<std::string>& variables);

    // print names of inner loops
    void printInnerLoops(std::ostream& os, const std::set<std::string>& loopNames);

  public:
    virtual void printProgram(std::ostream& os, Program* program, int indentLv = 0);
    virtual void printFunction(std::ostream& os, Function* function, int indentLv = 0);
    virtual void printLoop(std::ostream& os, Loop* loop, int indentLv = 0);
    virtual void printLoopPath(std::ostream& os, LoopPath* loopPath, int indentLv = 0);

    NormalPrinter() {}
    NormalPrinter(const std::string& indent): indent(indent) {}
    virtual ~NormalPrinter() {}
  };

  class JsonPrinter: public Printer {
  private:
    void printVarDecl(std::ostream& os, VariableTable* varTbl, int indentLv);
    void printVarInit(std::ostream& os, VariableTable* varTbl, int indentLv);
    void printFuncParam(std::ostream& os, const std::set<std::string>& params, int indentLv);
    void printProgramName(std::ostream& os, const std::string& name, int indentLv);

  public:
    virtual void printProgram(std::ostream& os, Program* program, int indentLv = 0);
    virtual void printFunction(std::ostream& os, Function* function, int indentLv = 0);
    virtual void printLoop(std::ostream& os, Loop* loop, int indentLv = 0);
    virtual void printLoopPath(std::ostream& os, LoopPath* loopPath, int indentLv = 0);

    virtual ~JsonPrinter() {}
  };
}

#endif
