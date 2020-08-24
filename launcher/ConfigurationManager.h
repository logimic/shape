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

#include "Component.h"
//#include "IConfigurationManager.h"
#include "IConfiguration.h"
#include "rapidjson/schema.h"
#include <set>
#include <map>
#include <mutex>

namespace shape {
  class Configuration;

  class ConfigurationManager
  {
  public:

    ConfigurationManager();
    virtual ~ConfigurationManager();

    IConfiguration* createConfiguration(const std::string& componentName, const std::string& instanceName);
    IConfiguration* getConfiguration(const std::string& componentName, const std::string& instanceName);
    std::vector<shape::IConfiguration*> listConfigurations(const std::string& componentName);

    void loadConfigurationSchemes();
    void loadExistingConfigurations();
    void setCfgDir(const std::string & cfgDir) { m_configurationDir = cfgDir; }
    void registerComponent(shape::Component* component);
    void unregisterComponent(shape::Component* component);

    void update(Configuration* cfg, bool persistent);
    void remove(Configuration* cfg);

  private:
    std::set<std::string> getConfigFiles(const std::string& dir) const;


    struct ComponentCfg
    {
      shape::Component* m_component = nullptr;
      std::map<std::string, std::shared_ptr<Configuration>> m_configs;
    };
    std::map<std::string, ComponentCfg> m_componentConfigs;

    std::mutex m_mtx;
    std::string m_configurationDir;

    std::map<std::string, rapidjson::SchemaDocument> m_validatorMap;

  };
}
