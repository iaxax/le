#ifndef LOOP_EXTRACTION_MESSAGE_H
#define LOOP_EXTRACTION_MESSAGE_H

#include <string>
#include <iostream>
#include <cstdlib>

namespace LE {

  // output message to user
  class Message {
  public:
    // output warning
    static inline void warning(const std::string& message) {
      std::cout << "LOOP EXTRACTION WARNING: " << message << std::endl;
    }

    // output error, process will be terminated
    static inline void error(const std::string& message) {
      std::cout << "LOOP EXTRACTION ERROR: " << message << std::endl;
      std::exit(1);
    }
  };

}

#endif
