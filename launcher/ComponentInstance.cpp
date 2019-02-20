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

#include "ComponentInstance.h"
#include "ProvidedInterface.h"
#include "RequiredInterface.h"

#include "Trace.h"
#include "ShapePropertiesMacros.h"
#include "ComponentManager.h"
#include "ComponentInstance.h"
#include "Component.h"

#ifdef SHAPE_PLATFORM_POSIX
#include <dlfcn.h>
#else
#include <Windows.h>
#endif

using namespace std;
using namespace shape;

namespace shape
{
  class ComponentInstanceStateConvertTable
  {
  public:
    static const std::vector<std::pair<ComponentInstance::MyState, std::string>>& table()
    {
      static std::vector <std::pair<ComponentInstance::MyState, std::string>> table = {
        { ComponentInstance::MyState::CREATE, "CREATE" },
        { ComponentInstance::MyState::INIT, "INIT" },
        { ComponentInstance::MyState::ACTIVE, "ACTIVE" },
        { ComponentInstance::MyState::FAILURE, "FAILURE" }
      };
      return table;
    }
    static ComponentInstance::MyState defaultEnum()
    {
      return ComponentInstance::MyState::CREATE;
    }
    static const std::string& defaultStr()
    {
      static std::string u("UNKNOWN");
      return u;
    }
  };

  typedef EnumStringConvertor<ComponentInstance::MyState, ComponentInstanceStateConvertTable> ComponentInstanceStateConvert;

  const std::string & ComponentInstance::getStateStr()
  {
    return ComponentInstanceStateConvert::enum2str(m_state);
  }

  ComponentInstance::MyState ComponentInstance::getState() const
  {
    return m_state;
  }

  std::ostream& operator<<(std::ostream& ostr, ComponentInstance::MyState state)
  {
    ostr << ComponentInstanceStateConvert::enum2str(state);
    return ostr;
  }

  ComponentInstance::ComponentInstance(const std::string& instanceName, const Component& component, std::shared_ptr<Properties> & props)
    : m_instanceName(instanceName)
    , m_component(component)
    , m_instance(nullptr)
  {
    TRC_FUNCTION_ENTER(PAR(instanceName));

    m_properties = props;

    //create instance from ComponentMeta
    ObjectTypeInfo* objectTypeInfo(nullptr);
    if (component.getComponentMeta()) {
      objectTypeInfo = component.getComponentMeta()->create();
      if (!objectTypeInfo) {
        THROW_EXC_TRC_WAR(std::logic_error, "objectTypeInfo nullptr");
      }
      m_instance = objectTypeInfo;
    }
    else {
      THROW_EXC_TRC_WAR(std::logic_error, "ComponentMeta nullptr");
    }

    //get provided interfaces
    for (auto it : component.getComponentMeta()->getProvidedInterfaceMap())
    {
      //get ObjectTypeInfo
      ObjectTypeInfo iface = it.second->getAsInterface(m_instance);

      //create provided interface
      std::shared_ptr<ProvidedInterface> providedInterface(
        shape_new ProvidedInterface(it.second, this, iface));

      //provided interfaces of this instance
      m_providedInterfaces.push_back(providedInterface);
    }

    //suports only flat properties as target
    std::vector<std::unique_ptr<Properties>> requiredInterfacesVect;
    std::map<std::string, std::unique_ptr<Properties>> targetPropsMap;
    if (m_properties) {
      GET_POSSIBLE_MEMBER_AS_VECTOR(*m_properties, Properties, "RequiredInterfaces", "", requiredInterfacesVect);
      for (auto & r : requiredInterfacesVect) {
        std::string ifname;
        std::unique_ptr<Properties> target;
        GET_MEMBER_AS(*r, String, "name", "", ifname);
        GET_MEMBER_AS(*r, Properties, "target", "", target);
        
        auto it = component.getComponentMeta()->getRequiredInterfaceMap().find(ifname);
        if (it != component.getComponentMeta()->getRequiredInterfaceMap().end()) {
          targetPropsMap[ifname] = std::move(target);
        }
        else {
          TRC_WARNING("Component instance" <<
            PAR(m_instanceName) <<
            "Targeted iface not found: " << PAR(ifname)
          );
        }
      }
    }

    //get required interfaces
    for (auto rq : component.getComponentMeta()->getRequiredInterfaceMap())
    {
      //check required interface targeted
      std::unique_ptr<Properties> targetProps;
      auto targetPropsIter = targetPropsMap.find(rq.first);
      if (targetPropsIter != targetPropsMap.end()) {
        targetProps = std::move(targetPropsIter->second);
      }

      std::shared_ptr<RequiredInterface> requiredInterface(shape_new RequiredInterface(rq.second, this, targetProps));

      //required interfaces of this instance
      m_requiredInterfaces.push_back(requiredInterface);
    }
     
    //pass reference to ComponentManager
    for (auto it : m_requiredInterfaces)
    {
      //announce required interfaces
      ComponentManager::getInst().addRequiredInterface(it);
    }

    m_state = MyState::CREATE;

    checkActivate();

    TRC_FUNCTION_LEAVE(PAR(instanceName));
  }

  ComponentInstance::~ComponentInstance()
  {
    TRC_FUNCTION_ENTER(NAME_PAR(instanceName, getInstanceName()));

    // remove provided interfaces
    for (auto provIface : m_providedInterfaces) {
      ComponentManager::getInst().removeProvidedInterface(provIface);
    }
    
    m_providedInterfaces.clear();

    try {
      if (m_state == MyState::ACTIVE) {
        m_component.getComponentMeta()->deactivate(m_instance);
      }
    }
    catch (std::exception &e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "When calling deactivate() of: " << NAME_PAR(instanceName, getInstanceName()));
    }
    catch (...) {
      TRC_WARNING("Caught unknown exception when calling deactivate() of: " << NAME_PAR(instanceName,getInstanceName()) << endl <<
        "The only std::exception is known here. The component should treat its own exceptions in an appropriate way!" << endl);
    }

    // detach provided interfaces from requirers
    for (auto & sr : m_requiredInterfaces) {
      sr->detachAllProvidedInterfaces();
    }

    //remove required interfaces
    while (!m_requiredInterfaces.empty()) {
      auto reqIfc = m_requiredInterfaces.begin();

      //retract required interfaces from public
      ComponentManager::getInst().removeRequiredInterface(*reqIfc);

      m_requiredInterfaces.erase(reqIfc);
    }

    try {
      m_component.getComponentMeta()->destroy(m_instance);
    }
    catch (std::exception &e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "When calling destroy() of: " << NAME_PAR(instanceName, getInstanceName()));
    }
    catch (...) {
      TRC_WARNING("Caught unknown exception when calling destroy() of: " << NAME_PAR(instanceName, getInstanceName()) << endl <<
        "Untreated component's exception!" << endl);
    }

    m_instance = nullptr;

    TRC_FUNCTION_LEAVE(NAME_PAR(instanceName, getInstanceName()));
  }

  void ComponentInstance::attachProvidedInterface(const std::shared_ptr<ProvidedInterface>& providedInterface, bool multiple)
  {
    TRC_FUNCTION_ENTER(NAME_PAR(instanceName, getInstanceName()) << PAR(*providedInterface) << PAR(multiple));

    TRC_INFORMATION(NAME_PAR(instanceName, getInstanceName()) << " gets service: " << PAR(*providedInterface));

    try {
      const RequiredInterfaceMeta* shapeRequiredInterface = m_component.getComponentMeta()->getRequiredInterface(providedInterface->getProvidedInterfaceObject()->getName());
      if (shapeRequiredInterface == nullptr) {
        THROW_EXC_TRC_WAR(std::logic_error, "shapeRequiredInterface is nullptr");
      }
      shapeRequiredInterface->attachInterface(m_instance, providedInterface->getProvidedInterfaceObject());

    }
    catch (std::exception &e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "When calling attachInterface(): " << PAR(*providedInterface) << NAME_PAR(instanceName, getInstanceName()));
    }
    catch (...) {
      TRC_WARNING("Caught unknown exception when calling attachInterface(): " << PAR(*providedInterface) << NAME_PAR(instanceName, getInstanceName()) << endl <<
        "Untreated component's exception!" << endl);
    }

    checkActivate();

    TRC_FUNCTION_LEAVE(NAME_PAR(instanceName, getInstanceName()) << PAR(m_state));
  }

  void ComponentInstance::detachProvidedInterface(const std::shared_ptr<ProvidedInterface>& providedInterface, bool multiple)
  {
    TRC_FUNCTION_ENTER(NAME_PAR(instanceName, getInstanceName()) << PAR(*providedInterface) << PAR(multiple));

    TRC_INFORMATION(NAME_PAR(instanceName, getInstanceName()) << " ungets service: " << PAR(*providedInterface));

    checkDeactivate();
    
    try {
      const RequiredInterfaceMeta* shapeRequiredInterface = m_component.getComponentMeta()->getRequiredInterface(providedInterface->getProvidedInterfaceObject()->getName());
      if (shapeRequiredInterface == nullptr) {
        THROW_EXC_TRC_WAR(std::logic_error, "shapeRequiredInterface is nullptr");
      }
      shapeRequiredInterface->detachInterface(m_instance, providedInterface->getProvidedInterfaceObject());

    }
    catch (std::exception &e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "When calling detachInterface(): " << PAR(*providedInterface) << NAME_PAR(instanceName, getInstanceName()));
    }
    catch (...) {
      TRC_WARNING("Caught unknown exception when calling detachInterface(): " << PAR(*providedInterface) << NAME_PAR(instanceName, getInstanceName()) << endl <<
        "Untreated component's exception!" << endl);
    }
    
    TRC_FUNCTION_LEAVE(NAME_PAR(instanceName, getInstanceName()) << PAR(m_state));
  }

  void ComponentInstance::checkActivate()
  {
    TRC_FUNCTION_ENTER(NAME_PAR(instanceName, getInstanceName()));
    
    if (m_state == MyState::CREATE) {

      bool satisfied = true;
      for (auto req : getRequiredInterfaces()) {
        if (!req->isSatisfied()) {
          satisfied = false;
          break;
        }
      }

      if (satisfied && m_state != MyState::ACTIVE && m_state != MyState::INIT) {
        try {
          m_state = MyState::INIT;
          m_component.getComponentMeta()->activate(m_instance, &getProperties());
          m_state = MyState::ACTIVE;

          //publish provided interfaces
          for (auto provIface : m_providedInterfaces) {
            ComponentManager::getInst().addProvidedInterface(provIface);
          }
        }
        catch (std::exception &e) {
          CATCH_EXC_TRC_WAR(std::exception, e, "When calling activate() of: " << NAME_PAR(instanceName, getInstanceName()));
          m_state = MyState::FAILURE;
        }
        catch (...) {
          TRC_WARNING("Caught unknown exception when calling activate() of : " << NAME_PAR(instanceName, getInstanceName()) << endl <<
            "Untreated component's exception!" << endl);
          m_state = MyState::FAILURE;
        }

      }
    }

    TRC_FUNCTION_LEAVE(NAME_PAR(instanceName, getInstanceName()));
  }

  void ComponentInstance::checkDeactivate()
  {
    TRC_FUNCTION_ENTER(NAME_PAR(instanceName, getInstanceName()));

    //is satisfied?
    //bool satisfied = true;
    //for (auto req : getRequiredInterfaces()) {
    //  if (!req->isSatisfied()) {
    //    satisfied = false;
    //    break;
    //  }
    //}

    if (m_state == MyState::ACTIVE)
    {
      m_state = MyState::CREATE;
      
      //unpublish provided interfaces
      for (auto provIface : m_providedInterfaces) {
        ComponentManager::getInst().removeProvidedInterface(provIface);
      }
      m_component.getComponentMeta()->deactivate(m_instance);
    }

    TRC_FUNCTION_LEAVE(NAME_PAR(instanceName, getInstanceName()));
  }

  void ComponentInstance::modify(std::shared_ptr<Properties> & props)
  {
    TRC_FUNCTION_ENTER("");
    
    try {
      m_component.getComponentMeta()->modify(m_instance, props.get());
    }
    catch (std::exception &e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "When calling modify() of: " << NAME_PAR(instanceName, getInstanceName()));
    }
    catch (...) {
      TRC_WARNING("Caught unknown exception when calling modify() of : " << NAME_PAR(instanceName, getInstanceName()) << endl <<
        "Untreated component's exception!" << endl);
    }

    TRC_FUNCTION_LEAVE("");
  }
}
