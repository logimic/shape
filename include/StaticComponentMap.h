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
