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

#include "StaticComponentMap.h"
#include "Component.h"
#include "ComponentManager.h"

#include "ShapeDefines.h"
#include "ShapePropertiesMacros.h"
#include "Trace.h"
#include <algorithm>

using namespace std;
using namespace shape;

namespace shape {

  class ComponentStateConvertTable
  {
  public:
    static const std::vector<std::pair<Component::MyState, std::string>>& table()
    {
      static std::vector <std::pair<Component::MyState, std::string>> table = {
        { Component::MyState::OPENED, "OPENED" },
        { Component::MyState::CLOSED, "CLOSED" },
        { Component::MyState::STARTING, "STARTING" },
        { Component::MyState::STARTED, "STARTED" },
        { Component::MyState::STOPPING, "STOPPING" }
      };
      return table;
    }
    static Component::MyState defaultEnum()
    {
      return Component::MyState::OPENED;
    }
    static const std::string& defaultStr()
    {
      static std::string u("UNKNOWN");
      return u;
    }
  };

  typedef EnumStringConvertor<Component::MyState, ComponentStateConvertTable> ComponentStateConvert;

  const std::string & Component::getStateStr()
  {
    return ComponentStateConvert::enum2str(m_state);
  }
  
  Component::MyState Component::getState() const
  {
    return m_state;
  }

  std::ostream& operator<<(std::ostream& ostr, Component::MyState state)
  {
    ostr << ComponentStateConvert::enum2str(state);
    return ostr;
  }

  //TODO component name from ComponentMeta
  Component::Component(
    const std::string & componentName,
    const std::string & libraryPath,
    const std::string & libraryName,
    const std::string & deploymentDir)
      : m_state(MyState::CLOSED)
    , m_componentName(componentName)
  {
    TRC_FUNCTION_ENTER(PAR(componentName) << PAR(libraryPath) << PAR(libraryName) << PAR(deploymentDir));
    
    m_libLoader.findLibrary(deploymentDir, libraryPath, libraryName);

    TRC_FUNCTION_LEAVE("");
  }

  Component::Component(const ComponentMeta * componentMeta)
    : m_componentMeta(componentMeta)
    , m_componentName(componentMeta->getComponentName())
  {
  }

  Component::~Component()
  {
    TRC_FUNCTION_ENTER(PAR(m_componentName) << PAR(m_componentName));
    removeInstances();
    TRC_FUNCTION_LEAVE(PAR(m_componentName));
  }

  void Component::setState(MyState state)
  {
    m_state = state;
    TRC_INFORMATION(PAR(m_componentName) << PAR(m_state));
  }

  void Component::openLibrary()
  {
    TRC_FUNCTION_ENTER(PAR(m_componentName) << PAR(m_state));

    if (MyState::CLOSED == m_state)
    {
      m_componentMeta = ComponentMap::get().getComponent(m_componentName);
      if (!m_componentMeta) {

        if (!m_libLoader.isOpenedLibrary()) {
          m_libLoader.openLibrary();
        }

        string functionName("get_component_");
        functionName.append(m_componentName);
        replace(functionName.begin(), functionName.end(), ':', '_');

        TRC_DEBUG("Calling: " << PAR(functionName));

        void* component = nullptr;
        unsigned long compiler = 0;
        size_t hashcode = 0;
        unsigned long expectedCompiler = (unsigned long)SHAPE_PREDEF_COMPILER;
        size_t expectedHashcode = std::type_index(typeid(ComponentMeta)).hash_code();

        // check loader
        if (!m_libLoader.isOpenedLibrary()) {
          std::vector<std::string> paths = m_libLoader.getTriedPaths();
          std::ostringstream os;
          for (const auto& p : paths) {
            os << p << std::endl;
          }
          THROW_EXC_TRC_WAR(std::logic_error, "Component library is not loaded: " << PAR(m_componentName) << std::endl <<
            "tried paths: " << os.str()
          );
        }
          
        // check loader result
        GetMetaComponent func = (GetMetaComponent)m_libLoader.getFunction(functionName);
        if (nullptr == func) {
          THROW_EXC_TRC_WAR(std::logic_error, "Cannot load library: " << PAR(m_componentName) << PAR(functionName));
        }
          
        // get component
        component = func(&compiler, &hashcode);

        // check component 
        if (nullptr == component) {
          THROW_EXC_TRC_WAR(std::logic_error, "Component is nullptr: " << PAR(m_componentName));
        }

        // check compiler
        if (compiler != expectedCompiler) {
          THROW_EXC_TRC_WAR(std::logic_error, "Unexpected compiler: " << PAR(m_componentName) <<
            PAR(expectedCompiler) <<
            PAR(compiler));
        }

        // check type ComponentMeta
        if (hashcode != expectedHashcode) {
          THROW_EXC_TRC_WAR(std::logic_error, "Unexpected type: " << PAR(m_componentName) <<
            PAR(expectedHashcode) <<
            PAR(hashcode));
        }

        m_componentMeta = (const ComponentMeta*)(component);
      }
      setState(MyState::OPENED);
    }
    TRC_FUNCTION_LEAVE(PAR(m_componentName) << PAR(m_state));
  }

  void Component::closeLibrary()
  {
    TRC_FUNCTION_ENTER(PAR(m_componentName) << PAR(m_state));

    if (MyState::STARTED == m_state)
    {
      stopInstances();
    }
    if (MyState::OPENED == m_state) {

      if (m_libLoader.isOpenedLibrary())
        m_libLoader.closeLibrary();

      setState(MyState::CLOSED);

    }

    TRC_FUNCTION_LEAVE(PAR(m_componentName) << PAR(m_state));
  }

  void Component::startInstances()
  {
    TRC_FUNCTION_ENTER(PAR(m_componentName) << PAR(m_state));

    if (MyState::CLOSED == m_state)
    {
      openLibrary();
    }
    if (MyState::OPENED == m_state)
    {

      setState(MyState::STARTING);
      bool err(false);

      try {
        ComponentManager::getInst().getConfigurationMgr().registerComponent(this);
      }
      catch (std::exception & e) {
        CATCH_EXC_TRC_WAR(std::exception, e, "Cannot start: " << PAR(m_componentName));
        setState(MyState::OPENED);
        err = true;
      }

      if (!err)
        setState(MyState::STARTED);

    }

    TRC_FUNCTION_LEAVE(PAR(m_componentName) << PAR(m_state));
  }

  void Component::stopInstances()
  {
    TRC_FUNCTION_ENTER(PAR(m_componentName) << PAR(m_state));

    if (MyState::STARTED == m_state)
    {

      setState(MyState::STOPPING);

      removeInstances();
      ComponentManager::getInst().getConfigurationMgr().unregisterComponent(this);

      setState(MyState::OPENED);

    }

    TRC_FUNCTION_LEAVE(PAR(m_componentName) << PAR(m_state));
  }

  const std::string& Component::getLibraryPathFullName() const
  {
    return m_libLoader.getLibraryPathName();
  }

  void Component::removeInstances()
  {
    TRC_FUNCTION_ENTER(PAR(m_componentName));

    while (!m_instances.empty())
    {
      auto it = m_instances.begin();
      m_instances.erase(it);
    }

    TRC_FUNCTION_LEAVE(PAR(m_componentName));
  }

  void Component::updateInstance(const std::string& id, std::shared_ptr<Properties> & props)
  {
    TRC_FUNCTION_ENTER(PAR(m_componentName) << PAR(id));
    std::shared_ptr<ComponentInstance> instance;
    string instId;

    //TODO data consistent id instId remove duplicit handling
    auto found = m_instances.find(id);
    if (m_instances.end() == found) {
      //doesn't exists, create it
      
      // CONFIG_INSTANCE_STR can be part of cfg already?
      try {
        GET_POSSIBLE_MEMBER_AS(*props, String, CONFIG_INSTANCE_STR, "", instId);
      }
      catch (std::exception &e) {
        CATCH_EXC_TRC_WAR(std::exception, e, "");
      }

      // check if exists
      if (!instId.empty()) {
        auto found = m_instances.find(instId);
        if (m_instances.end() != found) {
          //exists, cannot create with the same pid
          THROW_EXC_TRC_WAR(std::logic_error, "Unexpected duplicit configuration to create: " << PAR(instId));
        }
      }
      else {
        // use passed one
        instId = id;
      }

      std::shared_ptr<ComponentInstance> instance(shape_new ComponentInstance(id, *this, props));
      m_instances.insert(make_pair(instId, instance));

      TRC_DEBUG("Created instance for configuration: " << PAR(instId));
    }
    else {
      instance = found->second;
      //invokes "modify" of the instance
      instance->modify(props);
    }

    TRC_FUNCTION_LEAVE(PAR(instId));
  }

  void Component::removeInstance(const std::string& id)
  {
    TRC_FUNCTION_ENTER(PAR(m_componentName) << PAR(id));
    auto found = m_instances.find(id);
    if (m_instances.end() == found) {
      TRC_WARNING("Configuration to be deleted doesn't exists: " << PAR(id));
    }
    else {
      std::shared_ptr<ComponentInstance> instance = found->second;
      m_instances.erase(found);
    }
    TRC_FUNCTION_LEAVE(PAR(m_componentName))
  }

}
