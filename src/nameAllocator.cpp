#include "nameAllocator.h"

namespace LE {

  int LoopNameAllocator::id = 1;
  int PathNameAllocator::id = 1;
  int BlockNameAllocator::id = 1;

  static std::string allocName(const std::string& prefix, int& id) {
    std::stringstream ss;
    ss << prefix;
    ss << std::to_string(id++);
    return ss.str();
  }

  std::string LoopNameAllocator::allocName() {
    return LE::allocName("loop", LoopNameAllocator::id);
  }

  std::string PathNameAllocator::allocName() {
    return LE::allocName("path", PathNameAllocator::id);
  }

  std::string BlockNameAllocator::allocName() {
    return LE::allocName("block", BlockNameAllocator::id);
  }
}
