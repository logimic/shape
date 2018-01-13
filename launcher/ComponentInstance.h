#pragma once

#include "LibLoader.h"
#include "ShapeProperties.h"
#include "ShapeComponent.h"

#include <map>
#include <vector>
#include <memory>

namespace shape
{
  class Component;
  class ProvidedInterface;
  class RequiredInterface;
  
  class ComponentInstance
  {
  public:
    ComponentInstance(const std::string& componentName, const Component& component, std::shared_ptr<Properties> & props);
    virtual ~ComponentInstance();
    const std::vector<std::shared_ptr<RequiredInterface>>& getRequiredInterfaces() const { return m_requiredInterfaces; }
    const std::vector<std::shared_ptr<ProvidedInterface>>& getProvidedInterfaces() const { return m_providedInterfaces; }
    const Properties& getProperties() const { return *m_properties; }
    const Component& getComponent() const { return m_component; }
    const ObjectTypeInfo* getInstanceObject() const { return m_instance; }
    const std::string& getInstanceName() const { return m_instanceName; }
    void modify(std::shared_ptr<Properties> & props);
    void attachProvidedInterface(const std::shared_ptr<ProvidedInterface>& providedInterface, bool multiple);
    void detachProvidedInterface(const std::shared_ptr<ProvidedInterface>& providedInterface, bool multiple);

    void checkActivate();
    void checkDeactivate();

    enum class MyState {
      UNKNOWN,
      CREATE,
      INIT,
      FAILURE,
      ACTIVE
    };

    const std::string & getStateStr();
    MyState getState() const;

  private:
    std::vector<std::shared_ptr<ProvidedInterface>> m_providedInterfaces;
    std::vector<std::shared_ptr<RequiredInterface>> m_requiredInterfaces;
    std::shared_ptr<Properties> m_properties;
    const Component & m_component;
    ObjectTypeInfo* m_instance;
    std::string m_instanceName;
    MyState m_state = MyState::UNKNOWN;

  };
}
