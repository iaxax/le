#ifndef LOOP_EXTRACTION_CONSTRAINT_H
#define LOOP_EXTRACTION_CONSTRAINT_H

#include "rose.h"
#include <vector>

namespace LE {

  class ConstraintList {
  private:
    std::vector<SgExpression*> constraints;

  public:
    inline void addConstraint(SgExpression* expr) {
      constraints.push_back(expr);
    }

    inline std::vector<SgExpression*> getConstraints() const {
      return constraints;
    }

    ConstraintList() {}
    ConstraintList(const std::vector<SgExpression*>& c): constraints(c) {}

    ConstraintList* clone() const;

    typedef std::vector<SgExpression*>::iterator iterator;
    typedef std::vector<SgExpression*>::const_iterator const_iterator;

    inline iterator begin() { return constraints.begin(); }
    inline iterator end() { return constraints.end(); }
    inline const_iterator begin() const { return constraints.begin(); }
    inline const_iterator end() const { return constraints.end(); }
  };

}

#endif
