#ifndef LOOP_EXTRACTION_PRINTER_H
#define LOOP_EXTRACTION_PRINTER_H

#include "program.h"
#include <ostream>
#include <string>
#include <set>

namespace LE {

  class Printer {
  public:
    virtual void printProgram(std::ostream& os, Program* program) = 0;
    virtual void printFunction(std::ostream& os, Function* function) = 0;
    virtual void printLoop(std::ostream& os, Loop* loop) = 0;
    virtual void printLoopPath(std::ostream& os, LoopPath* loopPath) = 0;
    virtual void printExpression(std::ostream& os, SgExpression* expr);
    virtual ~Printer() = 0;
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
    virtual void printProgram(std::ostream& os, Program* program);
    virtual void printFunction(std::ostream& os, Function* function);
    virtual void printLoop(std::ostream& os, Loop* loop);
    virtual void printLoopPath(std::ostream& os, LoopPath* loopPath);

    NormalPrinter(): indent("    ") {}
    NormalPrinter(const std::string& indent): indent(indent) {}
    virtual ~NormalPrinter() {}
  };

  class JsonPrinter: public Printer {
  public:
    virtual void printProgram(std::ostream& os, Program* program);
    virtual void printFunction(std::ostream& os, Function* function);
    virtual void printLoop(std::ostream& os, Loop* loop);
    virtual void printLoopPath(std::ostream& os, LoopPath* loopPath);

    virtual ~JsonPrinter() {}
  };
}

#endif
