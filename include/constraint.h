#ifndef LOOP_EXTRACTION_CONSTRAINT_H
#define LOOP_EXTRACTION_CONSTRAINT_H

#include "rose.h"
#include <vector>

namespace LE {

  // constraint means that a condition must be met
  // when you want to execute a path in program
  // e.g if (i > 1) 'i > 1' is a condition
  //
  // ConstraintList represents a list of constraint
  // that must all be met in a path
  class ConstraintList {
  private:
    // list of constraints
    std::vector<SgExpression*> constraints;

  public:
    // add a constraint
    inline void addConstraint(SgExpression* expr) {
      constraints.push_back(expr);
    }

    ConstraintList() {}
    ConstraintList(const std::vector<SgExpression*>& c): constraints(c) {}

    // clone of a ConstraintList
    // note: elements(SgExpression*) in constraints are copied shallowly
    inline ConstraintList* clone() const {return new ConstraintList(constraints);}

    typedef std::vector<SgExpression*>::iterator iterator;
    typedef std::vector<SgExpression*>::const_iterator const_iterator;

    inline iterator begin() { return constraints.begin(); }
    inline iterator end() { return constraints.end(); }
    inline const_iterator begin() const { return constraints.begin(); }
    inline const_iterator end() const { return constraints.end(); }
  };

}

#endif
