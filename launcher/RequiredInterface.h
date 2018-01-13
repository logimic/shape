#pragma once

#include <ShapeComponent.h>
#include <vector>
#include <memory>

namespace shape
{
  class ProvidedInterface;

  class RequiredInterface
  {
  public:
    RequiredInterface() = delete;
    
    RequiredInterface(
      const RequiredInterfaceMeta* requiredInterfaceMeta,
      ComponentInstance* requirerInstance,
      std::unique_ptr<Properties> & targetProps);

    virtual ~RequiredInterface();
    const Properties * getTargetProps() const { return m_targetProps.get(); }
    const RequiredInterfaceMeta* getRequiredIntefaceMeta() const { return m_requiredInterfaceMeta; }
    const std::vector<std::shared_ptr<ProvidedInterface>> & getAttachedProvidedInterfaces() const { return m_providedInterfaces; }
    virtual void detachProvidedInterface(const std::shared_ptr<ProvidedInterface>& providedInterface);
    void detachAllProvidedInterfaces();
    virtual void attachProvidedInterface(const std::shared_ptr<ProvidedInterface>& providedInterface);
    bool operator == (const RequiredInterface& other) const;
    bool isSatisfied() const;
    friend std::ostream& operator<< (std::ostream& o, const RequiredInterface& ri);
  private:
    std::vector<std::shared_ptr<ProvidedInterface>> m_providedInterfaces;
    const RequiredInterfaceMeta* m_requiredInterfaceMeta;
    ComponentInstance* m_requirerInstance = nullptr;
    std::unique_ptr<Properties> m_targetProps;
  };

}
