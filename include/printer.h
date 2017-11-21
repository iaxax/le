#ifndef LOOP_EXTRACTION_PRINTER_H
#define LOOP_EXTRACTION_PRINTER_H

#include "loop.h"
#include <ostream>
#include <string>
#include <set>

namespace LE {

  // print result of loop extraction
  class Printer {
  private:
    // indentation for pretty printing
    static const std::string indent;

    // print update of variables in a path
    // see README for more detail
    static void printVariableUpdate(std::ostream& os, VariableTable* varTbl);

    // print constraints of a path
    // see README for more detail
    static void printConstraint(std::ostream& os, ConstraintList* cl);

    // print variables involved in a loop
    // see README for more detail
    static void printVarInvolved(std::ostream& os,
                                const std::set<std::string>& variables);

    // print names of inner loops
    static void printInnerLoops(std::ostream& os,
                                const std::set<std::string>& loopNames);

    // print a path, see README for more detail
    static void printPath(std::ostream& os, LoopPath* path);

  public:
    // print expression, transfer AST to infix expression
    static void printExpression(std::ostream& os, SgExpression* expr);

    // print a loop, see README for more detail
    static void print(std::ostream& os, Loop* loop);
  };
}

#endif
