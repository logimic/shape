#pragma once

#include "ComponentInstance.h"
#include <ShapeComponent.h>
#include <string>
#include <memory>

namespace shape
{
  class Component
  {
  public:
    Component() = delete;

    //TODO component name from ComponentMeta
    Component(
      const std::string & componentName,
      const std::string & libraryPath,
      const std::string & libraryName,
      const std::string & deploymentDir);

    Component(const ComponentMeta * componentMeta);

    virtual ~Component();

    const std::string& getLibraryPathFullName() const;
    const ComponentMeta* getComponentMeta() const { return m_componentMeta; }
    const std::string& getComponentName() const { return m_componentName; }
    const std::map<std::string, std::shared_ptr<ComponentInstance>> & getComponentInstances() const { return m_instances; }

    void openLibrary();
    void closeLibrary();
    void stopInstances();
    void startInstances();

    void updateInstance(const std::string& id, std::shared_ptr<Properties> & props);
    void removeInstance(const std::string& id);
    void removeInstances();

    enum class MyState {
      CLOSED,
      OPENED,
      STARTING,
      STARTED,
      STOPPING,
    };

    const std::string & getStateStr(); 
    MyState getState() const;

  private:
    LibLoader  m_libLoader;
    MyState m_state = MyState::CLOSED;
    void setState(MyState state);
    const ComponentMeta* m_componentMeta = nullptr;

    std::map<std::string, std::shared_ptr<ComponentInstance>> m_instances;
    std::string m_componentName;
  
  };

}
