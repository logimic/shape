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

#pragma once

#include "ShapeDefines.h"
#include <vector>
#include <string>

#ifdef ComponentService_EXPORTS
#define COMPONENT_SERVICE_DECLSPEC SHAPE_ABI_EXPORT
#else
#define COMPONENT_SERVICE_DECLSPEC SHAPE_ABI_IMPORT
#endif

namespace shape {

  class COMPONENT_SERVICE_DECLSPEC ILaunchService
  {
  public:
    virtual const std::vector<std::string>& getCommandLine() const = 0;
    virtual const std::string& getDeploymentDir() const = 0;
    virtual const std::string& getConfigurationDir() const = 0;
    virtual const std::string& getResourceDir() const = 0;
    virtual const std::string& getDataDir() const = 0;
    virtual const std::string& getCacheDir() const = 0;
    virtual const std::string& getUserDir() const = 0;
    virtual const std::string& getAppName() const = 0;

    virtual std::string componentOpenByName(const std::string& componentName, const std::string& libraryPath, const std::string& libraryName) = 0;
    virtual std::string componentOpen(int id) = 0;
    virtual std::string componentStart(int id) = 0;
    virtual std::string componentStop(int id) = 0;
    virtual std::string componentUnload(int id) = 0;

    virtual void upToRunning() = 0;
    virtual void exit() = 0;
    virtual void exit(bool wait) = 0;
    virtual bool isExiting() const = 0;
    virtual ~ILaunchService() {}
  };
}
