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
