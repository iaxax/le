#include "loopNameAllocator.h"

namespace LE {

  int LoopNameAllocator::id = 1;

  std::string LoopNameAllocator::allocLoopName() {
    std::stringstream ss;
    ss << "loop";
    ss << std::to_string(LoopNameAllocator::id++);
    return ss.str();
  }
}
