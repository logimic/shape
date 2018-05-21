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

#define configurationManager_EXPORTS
#include "ShapeDefines.h"
#include <iostream>

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
    TRC_FUNCTION_LEAVE("");
  }

  ConfigurationManager::~ConfigurationManager()
  {
    TRC_FUNCTION_ENTER("");
    TRC_FUNCTION_LEAVE("");
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
        const std::string& cname = cfg->getComponentName();

        auto validatorIt = m_validatorMap.find(cname);
        if (validatorIt != m_validatorMap.end()) {
          //found validator
          cfg->validate(validatorIt->second);
          if (cfg->isValidated()) {
            componentCfg.m_component->updateInstance(cfg->getId(), cfg->getProperties());
          }
          else {
            THROW_EXC_TRC_WAR(std::logic_error, "Configuration invalid according JSON schema: " << NAME_PAR(componentName, cfg->getComponentName()));
          }
        }
        else {
          THROW_EXC_TRC_WAR(std::logic_error, "Missing JSON schema for validation: " << NAME_PAR(componentName, cfg->getComponentName()));
        }
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
  void ConfigurationManager::loadConfigurationSchemes()
  {
    TRC_FUNCTION_ENTER("");
    using namespace rapidjson;

    std::string schemasDir = m_configurationDir + "/jsonschema";
    std::set<std::string> schemas;
    try {
      schemas = getConfigFiles(schemasDir);
    }
    catch (std::exception & e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "Cannot get cfg schemas fromm: " << PAR(schemasDir));
    }

    for (const auto & schemaFile : schemas) {
      try {
        Document sd;

        std::ifstream ifs(schemaFile);
        if (!ifs.is_open()) {
          THROW_EXC_TRC_WAR(std::logic_error, "Cannot open: " << PAR(schemaFile));
        }

        rapidjson::IStreamWrapper isw(ifs);
        sd.ParseStream(isw);

        if (sd.HasParseError()) {
          THROW_EXC_TRC_WAR(std::logic_error, "Json parse error: " << NAME_PAR(emsg, sd.GetParseError()) <<
            NAME_PAR(eoffset, sd.GetErrorOffset()));
        }

        Value *v = Pointer("/properties/component/enum/0").Get(sd);
        if (v && v->IsString()) {
          SchemaDocument schemaDoc(sd);
          m_validatorMap.insert(std::make_pair(v->GetString(), std::move(schemaDoc)));
        }
        else {
          THROW_EXC_TRC_WAR(std::logic_error, "JsonSchema error: " << PAR(schemaFile) << "missing: /properties/component/enum/0");
        }
      }
      catch (std::exception & e) {
        CATCH_EXC_TRC_WAR(std::exception, e, "Cannot create validator from: " << PAR(schemaFile));
      }
    }

    TRC_FUNCTION_LEAVE("");
  }

  //----------------------------------------------------
  void ConfigurationManager::loadExistingConfigurations()
  {
    TRC_FUNCTION_ENTER("");

    std::set<std::string> files = getConfigFiles(m_configurationDir);
    for (auto it = files.begin(); it != files.end(); it++) {

      try {
        TRC_DEBUG("Checking file: " << NAME_PAR(fname, *it));
        shared_ptr<Configuration> shpc(shape_new Configuration(*it, this));
        const std::string& cname = shpc->getComponentName();

        auto validatorIt = m_validatorMap.find(cname);
        if (validatorIt != m_validatorMap.end()) {
          //found validator
          shpc->validate(validatorIt->second);
        }

        auto found = m_componentConfigs[cname].m_configs.find(shpc->getId());
        if (found != m_componentConfigs[cname].m_configs.end()) {
          THROW_EXC_TRC_WAR(std::logic_error, "Cannot insert probably duplicit configuration id: " <<
            NAME_PAR(id, shpc->getId()) << NAME_PAR(file, *it));
        }
        m_componentConfigs[cname].m_configs[shpc->getId()] = shpc;

      }
      catch (std::exception & e) {
        std::cerr << "Cannot process: " << NAME_PAR(fname, *it) << std::endl;
        CATCH_EXC_TRC_WAR(std::exception, e, "");
      }
    }

    TRC_FUNCTION_LEAVE("");
  }

#ifdef SHAPE_PLATFORM_WINDOWS
  std::set<std::string> ConfigurationManager::getConfigFiles(const std::string& dir) const
  {
    WIN32_FIND_DATA fid;
    HANDLE found = INVALID_HANDLE_VALUE;

    std::set<std::string>  fileSet;
    string sdirect(dir);
    sdirect.append("/*.json");

    found = FindFirstFile(sdirect.c_str(), &fid);

    if (INVALID_HANDLE_VALUE == found) {
      THROW_EXC_TRC_WAR(std::logic_error, "Directory does not exist: " << PAR(dir));
    }

    do {
      if (fid.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        continue; //skip a directory
      string fil(dir);
      fil.append("/");
      fil.append(fid.cFileName);
      fileSet.insert(fil);
    } while (FindNextFile(found, &fid) != 0);

    FindClose(found);
    return fileSet;
  }

#else
  std::set<std::string> ConfigurationManager::getConfigFiles(const std::string& dirStr) const
  {
    std::set<std::string> fileSet;

    DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(dirStr.c_str());
    if (dir == nullptr) {
      THROW_EXC_TRC_WAR(std::logic_error, "Directory does not exist: " << PAR(dirStr));
    }
    //TODO exeption if dir doesn't exists
    while ((ent = readdir(dir)) != NULL) {
      const string file_name = ent->d_name;
      const string full_file_name(dirStr + "/" + file_name);

      if (file_name[0] == '.')
        continue;

      if (stat(full_file_name.c_str(), &st) == -1)
        continue;

      const bool is_directory = (st.st_mode & S_IFDIR) != 0;

      if (is_directory)
        continue;

      fileSet.insert(full_file_name);
    }
    closedir(dir);


    return fileSet;
  }

#endif

}


