#include "constraint.h"

namespace LE {

  ConstraintList* ConstraintList::clone() const {
    return new ConstraintList(constraints);
  }

}
