#ifndef LOOP_EXTRACTION_PRINTER_H
#define LOOP_EXTRACTION_PRINTER_H

#include "loop.h"
#include <ostream>
#include <string>

namespace LE {

  class Printer {
  private:
    static const std::string indent;

    static void printVariableUpdate(std::ostream& os, VariableTable* varTbl);
    static void printConstraint(std::ostream& os, ConstraintList* cl);
    static void printVarInvolved(std::ostream& os, Loop* loop);
    static void printPath(std::ostream& os, LoopPath* path);

  public:
    static void printExpression(std::ostream& os, SgExpression* expr);
    static void print(std::ostream& os, Loop* loop);
  };
}

#endif
