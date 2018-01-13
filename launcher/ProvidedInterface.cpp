#include "ComponentInstance.h"
#include "ProvidedInterface.h"
#include "Trace.h"

using namespace std;

namespace shape
{
  ProvidedInterface::ProvidedInterface(
    const shape::ProvidedInterfaceMeta* providedInterfaceMeta,
    ComponentInstance* providerInstance,
    const ObjectTypeInfo & providerInterfaceObject)
    :m_providerInstance(providerInstance)
    , m_providerInterfaceObject(providerInterfaceObject)
    , m_providedInterfaceMeta(providedInterfaceMeta)
  {
  }

  ProvidedInterface::~ProvidedInterface()
  {
  }
  
  bool ProvidedInterface::operator == (const ProvidedInterface& other) const
  {
    return m_providerInterfaceObject.getObject() == other.m_providerInterfaceObject.getObject();
  }

  bool ProvidedInterface::operator != (const ProvidedInterface& other) const
  {
    return m_providerInterfaceObject.getObject() != other.m_providerInterfaceObject.getObject();
  }

  bool ProvidedInterface::isTarget(const Properties * targetProps) const
  {
    if (targetProps)
      return targetProps->isSubsetOf(m_providerInstance->getProperties());
    else
      return true;
  }

  std::ostream& operator<< (std::ostream& o, const ProvidedInterface& pi)
  {
    o << 
      NAME_PAR(ProvidedInterface, pi.m_providerInterfaceObject.getName()) <<
      NAME_PAR(ProviderName, pi.getComponentInstance()->getInstanceName());
    return o;
  }

}
