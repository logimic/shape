/**
 * Copyright 2018 Logimic,s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Shaper.h"
#include "SignalHandler.h"
#include "ComponentManager.h"
#include "StaticComponents.h"
#include <iostream>
#include <vector>

int shapeInit(int argc, char** argv)
{
  staticInit();

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

  return retval;
}

int shapeRun()
{
  return shape::ComponentManager::getInst().run();
}

void shapeReady()
{
  shape::ComponentManager::getInst().upToRunning();
}
