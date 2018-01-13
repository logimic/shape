#include <Shaper.h>
#include "StaticComponents.h"
#include <Trace.h>
#include <iostream>

TRC_INIT("startup");

int main(int argc, char** argv)
{
  std::cout << "startup ... " << std::endl;
  staticInit();
  shapeInit(argc, argv);
  shapeRun();
  return 0;
}
