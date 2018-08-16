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

#include "ConfigurationManager.h"
#include "ComponentInstance.h"

#include <vector>
#include <set>
#include <map>
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>

namespace shape
{
  class ComponentManager
  {
  public:
    static ComponentManager& getInst();
    virtual ~ComponentManager();
    int init(const std::vector<std::string>& commandLine, PropertiesJson* props = nullptr);
    int run();

    const std::vector<std::string>& getCommandLine() const { return m_commandLine; }
    const std::string& getDeploymentDir() const { return m_deploymentDir; }
    const std::string& getConfigurationDir() const { return m_configurationDir; }
    const std::string& getResourceDir() const { return m_resourceDir; }
    const std::string& getDataDir() const { return m_dataDir; }
    const std::string& getCacheDir() const { return m_cacheDir; }
    const std::string& getUserDir() const { return m_userDir; }
    const std::string& getApplicationName() const { return m_applicationName; }

    virtual std::string componentOpenByName(
      const std::string& componentName,
      const std::string& libraryPath,
      const std::string& libraryName);
    virtual std::string componentOpen(int id);
    virtual std::string componentStart(int id);
    virtual std::string componentStop(int id);
    virtual std::string componentUnload(int id);

    virtual void exit();
    virtual void exit(int retval);
    virtual void exit(bool wait, int retval = 0);
    virtual bool isExiting() const;

    virtual void upToRunning();

    unsigned getPredefCompilerVer() const { return m_predefinedCompiler; }

    ConfigurationManager& getConfigurationMgr() { return m_configurationMgr; }

    void addProvidedInterface(std::shared_ptr<ProvidedInterface> providedInterface);
    void removeProvidedInterface(std::shared_ptr<ProvidedInterface> providedInterface);
    void addRequiredInterface(std::shared_ptr<RequiredInterface> requiredInterface);
    void removeRequiredInterface(std::shared_ptr<RequiredInterface> requiredInterface);
    std::shared_ptr<ProvidedInterface> getReplacingProvidedInterface(
      const RequiredInterface& requiredInterface, std::shared_ptr<ProvidedInterface>& currentProvidedInterface);
    std::shared_ptr<Component> createComponent(
      int startLvl,
      const std::string& componentName,
      const std::string& libraryPath,
      const std::string& libraryName);
  
    ComponentManager(const ComponentManager&) = delete;

  private:
    friend class LauncherService;

    ComponentManager();

    void startComponents();
    void eraseComponents();

    mutable std::recursive_mutex m_componentMtx;

    mutable std::mutex m_exitMtx;
    std::condition_variable m_exitCond;
    bool m_exit = false;
    bool m_exitRetval = 0;
    bool m_exitComplete = false;

    std::mutex m_runningMutex;
    std::condition_variable m_runningCondition;
    bool m_running = false;

    std::vector<std::string> m_commandLine;
    PropertiesJson m_properties;

    std::string m_deploymentDir;
    std::string m_configurationDir;
    std::string m_resourceDir;
    std::string m_dataDir;
    std::string m_cacheDir;
    std::string m_userDir;

    std::thread::id m_myThreadId;
    std::string m_applicationName;
    unsigned m_predefinedCompiler;

    ConfigurationManager m_configurationMgr;

    // components according index
    std::map<int, std::shared_ptr<Component>> m_indexComponents;

    // components according name
    std::map<std::string, std::shared_ptr<Component>> m_nameComponents;

    // disabled components names
    std::set<std::string> m_disabledComponents;

    /// provided interfaces
    std::multimap<std::type_index, std::shared_ptr<ProvidedInterface>> m_providedInterfaces;

    /// required interfaces
    std::multimap<std::type_index, std::shared_ptr<RequiredInterface>> m_requiredInterfaces;

  };

}
