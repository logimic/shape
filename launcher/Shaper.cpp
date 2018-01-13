#include "Shaper.h"
#include "SignalHandler.h"
#include "ComponentManager.h"
#include <iostream>
#include <vector>

int shapeInit(int argc, char** argv)
{
  int retval = 0;
  retval = shape::initSignals();
  if (retval != 0) {
    return -1;
  }

  std::vector<std::string> cmdlPars;
  for (int ia = 0; ia < argc; ia++) {
    cmdlPars.push_back(argv[ia]);
  }

  retval = shape::ComponentManager::getInst().init(cmdlPars);
  if (retval != 0) {
    return -1;
  }

}

void shapeRun()
{
  shape::ComponentManager::getInst().run();
}

void shapeReady()
{
  shape::ComponentManager::getInst().upToRunning();
}
