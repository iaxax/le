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
    static std::string allocName();
  };

  // allocate global unique name for path
  class PathNameAllocator {
  private:
    static int id;

  public:
    static std::string allocName();
  };

  // allocte global unique name for block
  class BlockNameAllocator {
  private:
    static int id;

  public:
    static std::string allocName();
  };
}

#endif
