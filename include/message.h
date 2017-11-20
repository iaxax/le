#ifndef LOOP_EXTRACTION_MESSAGE_H
#define LOOP_EXTRACTION_MESSAGE_H

#include <string>
#include <iostream>
#include <cstdlib>

namespace LE {

  class Message {
  public:
    static inline void warning(const std::string& message) {
      std::cout << "LOOP EXTRACTION WARNING: " << message << std::endl;
    }

    static inline void error(const std::string& message) {
      std::cout << "LOOP EXTRACTION ERROR: " << message << std::endl;
      std::exit(1);
    }
  };

}

#endif
