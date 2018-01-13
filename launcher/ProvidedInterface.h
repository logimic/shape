#pragma once

#include <ShapeComponent.h>
#include <typeindex>

namespace shape
{
  class ComponentInstance;

  class ProvidedInterface
  {
  public:
    ProvidedInterface() = delete;
    ProvidedInterface(
      const ProvidedInterfaceMeta* providedInterfaceMeta,
      ComponentInstance* providerInstance,
      const ObjectTypeInfo & providerInterfaceObject);

    virtual ~ProvidedInterface();
    const ComponentInstance* getComponentInstance() const { return m_providerInstance; }
    const ObjectTypeInfo* getProvidedInterfaceObject() const { return &m_providerInterfaceObject; }
    const ProvidedInterfaceMeta* getProvidedIntefaceMeta() const { return m_providedInterfaceMeta; }
    bool operator == (const ProvidedInterface& other) const;
    bool operator != (const ProvidedInterface& other) const;
    bool isTarget(const Properties * targetProps) const;
    friend std::ostream& operator<<(std::ostream& o, const ProvidedInterface& pi);
  private:
    ComponentInstance* m_providerInstance;
    ObjectTypeInfo m_providerInterfaceObject;
    const ProvidedInterfaceMeta* m_providedInterfaceMeta;
  };
}
