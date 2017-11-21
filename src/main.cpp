#include "loopExtraction.h"

int main(int argc, char *argv[]) {
  LE::LoopExtraction le(argc, argv);
  le.handleSgProject();
  return 0;
}
