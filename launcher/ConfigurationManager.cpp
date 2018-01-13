#define configurationManager_EXPORTS
#include "ShapeDefines.h"

#ifdef SHAPE_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#include "ConfigurationManager.h"
#include "Configuration.h"
#include "ShapeDefines.h"
#include "Trace.h"
#include <memory>

namespace shape
{
  using namespace std;

  ConfigurationManager::ConfigurationManager()
  {
    TRC_FUNCTION_ENTER("");
    TRC_FUNCTION_LEAVE("")
  }

  ConfigurationManager::~ConfigurationManager()
  {
    TRC_FUNCTION_ENTER("");
    TRC_FUNCTION_LEAVE("")
  }

  void ConfigurationManager::registerComponent(shape::Component* component)
  {
    TRC_FUNCTION_ENTER(NAME_PAR(componentName, component->getComponentName()));
    lock_guard<mutex> lck(m_mtx);

    auto found = m_componentConfigs.find(component->getComponentName());
    if (found != m_componentConfigs.end()) {
      ComponentCfg & componentCfg = found->second;
      if (componentCfg.m_component != nullptr) {
        THROW_EXC_TRC_WAR(std::logic_error, "Cannot insert duplicit: " << NAME_PAR(componentName, component->getComponentName()));
      }
      else {
        componentCfg.m_component = component;
        if (!componentCfg.m_configs.empty()) {
          //create instances according configurations
          for (auto & cfg : componentCfg.m_configs) {
            componentCfg.m_component->updateInstance(cfg.second->getId(), cfg.second->getProperties());
          }
        }
      }
    }
    TRC_FUNCTION_LEAVE("");
  }

  void ConfigurationManager::unregisterComponent(shape::Component* component)
  {
    TRC_FUNCTION_ENTER(NAME_PAR(componentName, component->getComponentName()));
    lock_guard<mutex> lck(m_mtx);

    m_componentConfigs[component->getComponentName()].m_component = nullptr;
    TRC_FUNCTION_LEAVE("");
  }

  IConfiguration* ConfigurationManager::createConfiguration(const std::string& componentName, const std::string& instanceName)
  {
    TRC_FUNCTION_ENTER(PAR(componentName) << PAR(instanceName));
    lock_guard<mutex> lck(m_mtx);
    IConfiguration* cfg(nullptr);

    auto found = m_componentConfigs.find(componentName);
    if (found == m_componentConfigs.end()) {
      THROW_EXC_TRC_WAR(std::logic_error, "Cannot find: " << PAR(componentName));
    }
    else {
      ComponentCfg & componentCfg = found->second;
      if (componentCfg.m_component == nullptr) {
        THROW_EXC_TRC_WAR(std::logic_error, "Component is not started: " << PAR(componentName));
      }
      else {
        shared_ptr<Configuration> shpc(shape_new Configuration(instanceName, componentName, this));
        auto result = componentCfg.m_configs.insert(make_pair(componentName, shpc));
        if (!result.second) {
          THROW_EXC_TRC_WAR(std::logic_error, "Cannot insert duplicit: " << PAR(instanceName));
        }
        cfg = shpc.get();
      }
    }

    TRC_FUNCTION_LEAVE(PAR(cfg));
    return cfg;
  }

  IConfiguration* ConfigurationManager::getConfiguration(const std::string& componentName, const std::string& instanceName)
  {
    TRC_FUNCTION_ENTER(PAR(componentName) << PAR(instanceName));
    lock_guard<mutex> lck(m_mtx);
    IConfiguration* retval(nullptr);

    auto compIt = m_componentConfigs.find(componentName);
    if (compIt != m_componentConfigs.end()) {
      auto cfgIt = compIt->second.m_configs.find(instanceName);
      if (cfgIt != compIt->second.m_configs.end()) {
        retval = cfgIt->second.get();
      }
    }
    TRC_FUNCTION_LEAVE(PAR(retval));
    return retval;
  }

  void ConfigurationManager::update(Configuration* cfg)
  {
    TRC_FUNCTION_ENTER(PAR(cfg));
    lock_guard<mutex> lck(m_mtx);

    auto found = m_componentConfigs.find(cfg->getComponentName());
    if (found == m_componentConfigs.end()) {
      THROW_EXC_TRC_WAR(std::logic_error, "Cannot find: " << NAME_PAR(componentName, cfg->getComponentName()));
    }
    else {
      ComponentCfg & componentCfg = found->second;
      if (componentCfg.m_component == nullptr) {
        THROW_EXC_TRC_WAR(std::logic_error, "Component is nullptr: " << NAME_PAR(componentName, cfg->getComponentName()));
      }
      else {
        componentCfg.m_component->updateInstance(cfg->getId(), cfg->getProperties());
      }
    }
    TRC_FUNCTION_LEAVE("");
  }

  void ConfigurationManager::remove(Configuration* cfg)
  {
    TRC_FUNCTION_ENTER(PAR(cfg));
    lock_guard<mutex> lck(m_mtx);

    auto found = m_componentConfigs.find(cfg->getComponentName());
    if (found == m_componentConfigs.end()) {
      THROW_EXC_TRC_WAR(std::logic_error, "Cannot find: " << NAME_PAR(componentName, cfg->getComponentName()));
    }
    else {
      ComponentCfg & componentCfg = found->second;
      if (componentCfg.m_component == nullptr) {
        THROW_EXC_TRC_WAR(std::logic_error, "Component is nullptr: " << NAME_PAR(componentName, cfg->getComponentName()));
      }
      else {
        componentCfg.m_component->removeInstance(cfg->getId());
        componentCfg.m_configs.erase(cfg->getId());
      }
    }
    TRC_FUNCTION_LEAVE("");
  }

  std::vector<IConfiguration*> ConfigurationManager::listConfigurations(const std::string& componentName)
  {
    TRC_FUNCTION_ENTER(PAR(componentName));
    vector<IConfiguration*> vect;
    if (componentName.empty()) {
      for (auto compIt : m_componentConfigs) {
        for (auto cfgIt : compIt.second.m_configs) {
          vect.push_back(cfgIt.second.get());
        }
      }
    }
    else {
      auto compIt = m_componentConfigs.find(componentName);
      if (compIt != m_componentConfigs.end()) {
        for (auto cfgIt : compIt->second.m_configs) {
          vect.push_back(cfgIt.second.get());
        }
      }
    }
    TRC_FUNCTION_LEAVE("");
    return vect;
  }

  //----------------------------------------------------
  void ConfigurationManager::loadExistingConfigurations()
  {
    TRC_FUNCTION_ENTER("");

    vector<string> files = getConfigFiles();

    for (auto it = files.begin(); it != files.end(); it++) {

      try {
        TRC_DEBUG("Checking file: " << NAME_PAR(fname, *it));
        shared_ptr<Configuration> shpc(shape_new Configuration(*it, this));

        auto found = m_componentConfigs[shpc->getComponentName()].m_configs.find(shpc->getId());
        if (found != m_componentConfigs[shpc->getComponentName()].m_configs.end()) {
          THROW_EXC_TRC_WAR(std::logic_error, "Cannot insert probably duplicit configuration id: " <<
            NAME_PAR(id, shpc->getId()) << NAME_PAR(file, *it));
        }
        m_componentConfigs[shpc->getComponentName()].m_configs[shpc->getId()] = shpc;

      }
      catch (std::exception & e) {
        CATCH_EXC_TRC_WAR(std::exception, e, "");
      }
    }

    TRC_FUNCTION_LEAVE("");
  }

#ifdef SHAPE_PLATFORM_WINDOWS
  vector<string> ConfigurationManager::getConfigFiles()
  {
    WIN32_FIND_DATA fid;
    HANDLE found = INVALID_HANDLE_VALUE;

    vector<string> fileVect;
    string sdirect(m_configurationDir);
    sdirect.append("/*.json");

    found = FindFirstFile(sdirect.c_str(), &fid);

    if (INVALID_HANDLE_VALUE == found) {
      THROW_EXC_TRC_WAR(std::logic_error, "Configuration directory does not exist: " << PAR(m_configurationDir));
    }

    do {
      if (fid.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        continue; //skip a directory
      string fil(m_configurationDir);
      fil.append("/");
      fil.append(fid.cFileName);
      fileVect.push_back(fil);
    } while (FindNextFile(found, &fid) != 0);

    FindClose(found);
    return fileVect;
  }

#else
  vector<string> ConfigurationManager::getConfigFiles()
  {
    vector<string> fileVect;

    DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(m_configurationDir.c_str());
    if (dir == nullptr) {
      THROW_EXC_TRC_WAR(std::logic_error, "Configuration directory does not exist: " << PAR(m_configurationDir));
    }
    //TODO exeption if dir doesn't exists
    while ((ent = readdir(dir)) != NULL) {
      const string file_name = ent->d_name;
      const string full_file_name(m_configurationDir + "/" + file_name);

      if (file_name[0] == '.')
        continue;

      if (stat(full_file_name.c_str(), &st) == -1)
        continue;

      const bool is_directory = (st.st_mode & S_IFDIR) != 0;

      if (is_directory)
        continue;

      fileVect.push_back(full_file_name);
    }
    closedir(dir);


    return fileVect;
  }

#endif

}


