#ifndef LOOP_EXTRACTION_LOOP_NAME_ALLOCATOR_H
#define LOOP_EXTRACTION_LOOP_NAME_ALLOCATOR_H

#include <sstream>
#include <string>

namespace LE {

  class LoopNameAllocator {
  private:
    static int id;
  public:
    static std::string allocLoopName();
  };
}

#endif
