#ifndef LOOP_EXTRACTION_LOOP_NAME_ALLOCATOR_H
#define LOOP_EXTRACTION_LOOP_NAME_ALLOCATOR_H

#include <sstream>
#include <string>

namespace LE {

  // allocate global unique name for loop
  class LoopNameAllocator {
  private:
    // id, every time allocLoopName() is called
    // it will increment
    static int id;

  public:
    // get a global unique name
    static std::string allocLoopName();
  };
}

#endif
