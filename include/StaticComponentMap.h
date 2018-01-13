#pragma once

#include <ShapeComponent.h>
#include "Trace.h"
#include <map>
#include <string>
#include <iostream>

namespace shape {
	class ComponentMap
  {
	public:
    static ComponentMap& get()
    {
      static ComponentMap s;
      return s;
    }
    virtual ~ComponentMap() {}
    
    void setComponent(const ComponentMeta* component)
    {
      auto result = m_componentMap.insert(make_pair(component->getComponentName(), component));
      if (!result.second) {
        std::cerr << NAME_PAR(componentName, component->getComponentName()) << ": is already stored. The binary isn't probably correctly linked" << std::endl;
        THROW_EXC_TRC_WAR(std::logic_error, NAME_PAR(componentName, component->getComponentName()) << ": is already stored. The binary isn't probably correctly linked");
      }
    }

    const ComponentMeta* getComponent(const std::string& componentName) const
    {
      auto result = m_componentMap.find(componentName);
      if (result != m_componentMap.end()) {
        return result->second;
      }
      return nullptr;
    }

  private:
    ComponentMap()
    {}
    std::map<std::string, const ComponentMeta*> m_componentMap;
  };

}
