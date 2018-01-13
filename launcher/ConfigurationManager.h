#pragma once

#include "Component.h"
#include "IConfigurationManager.h"
#include "IConfiguration.h"
#include <map>
#include <mutex>

namespace shape {
  class Configuration;

  class ConfigurationManager : public shape::IConfigurationManager
  {
  public:

    ConfigurationManager();
    virtual ~ConfigurationManager();

    IConfiguration* createConfiguration(const std::string& componentName, const std::string& instanceName) override;
    IConfiguration* getConfiguration(const std::string& componentName, const std::string& instanceName) override;
    std::vector<shape::IConfiguration*> listConfigurations(const std::string& componentName) override;

    void loadExistingConfigurations();
    void setCfgDir(const std::string & cfgDir) { m_configurationDir = cfgDir; }
    void registerComponent(shape::Component* component);
    void unregisterComponent(shape::Component* component);

    void update(Configuration* cfg);
    void remove(Configuration* cfg);

  private:
    std::vector<std::string> getConfigFiles();
    
    struct ComponentCfg
    {
      shape::Component* m_component = nullptr;
      std::map<std::string, std::shared_ptr<Configuration>> m_configs;
    };
    std::map<std::string, ComponentCfg> m_componentConfigs;

    std::mutex m_mtx;
    std::string m_configurationDir;

  };
}
