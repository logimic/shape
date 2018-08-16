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

#include "ILaunchService.h"
#include "ITraceService.h"
#include "ShapeProperties.h"

#ifndef launcher_EXPORT
#   define launcher_EXPORT
#endif

namespace shape
{
  class LauncherService : public ILaunchService
  {
  public:
    void activate(const Properties *props = 0);
    void deactivate();
    void modify(const Properties *props);

    //TODO
    void attachInterface(shape::ITraceService* iface);
    void detachInterface(shape::ITraceService* iface);

    //for ILaunchService
    const std::vector<std::string>& getCommandLine() const override;
    virtual const std::string& getDeploymentDir() const override;
    virtual const std::string& getConfigurationDir() const override;
    virtual const std::string& getResourceDir() const override;
    virtual const std::string& getDataDir() const override;
    virtual const std::string& getCacheDir() const override;
    virtual const std::string& getUserDir() const override;
    const std::string& getAppName() const override;

    std::string componentOpenByName(const std::string& componentName, const std::string& libraryPath, const std::string& libraryName) override;
    std::string componentOpen(int id) override;
    std::string componentStart(int id) override;
    std::string componentStop(int id) override;
    std::string componentUnload(int id) override;

    void exit() override;
    void exit(int retval) override;
    void exit(bool wait) override;
    bool isExiting() const override;
    void upToRunning() override;

  };
  
}
