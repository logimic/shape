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

#include "ComponentManager.h"
#include "ProvidedInterface.h"
#include "RequiredInterface.h"
#include "Trace.h"

using namespace std;

namespace shape
{
  RequiredInterface::RequiredInterface(
    const RequiredInterfaceMeta* requiredInterfaceMeta,
    ComponentInstance* requirerInstance,
    std::unique_ptr<Properties> & targetProps)
    : m_requiredInterfaceMeta(requiredInterfaceMeta)
    , m_requirerInstance(requirerInstance)
    , m_targetProps(std::move(targetProps))
  {
    TRC_FUNCTION_ENTER(*this);
    TRC_FUNCTION_LEAVE("");
  }

  RequiredInterface::~RequiredInterface()
  {
    TRC_FUNCTION_ENTER(*this);
    TRC_FUNCTION_LEAVE("");
  }

  void RequiredInterface::detachProvidedInterface(const std::shared_ptr<ProvidedInterface>& providedInterface)
  {
    TRC_FUNCTION_ENTER(PAR(*providedInterface));
    for (auto provIfaceIt = m_providedInterfaces.begin(); provIfaceIt != m_providedInterfaces.end(); ++provIfaceIt)
    {
      std::shared_ptr<ProvidedInterface>& provIface = *provIfaceIt;
      if (*provIface == *providedInterface)
      {
        m_providedInterfaces.erase(provIfaceIt);

        if (!m_requiredInterfaceMeta->getPolicyStatic() && Cardinality::SINGLE == m_requiredInterfaceMeta->getCardinality()) //DYNAMIC && SINGLE
        {
          std::shared_ptr<ProvidedInterface> replace =
            ComponentManager::getInst().getReplacingProvidedInterface(*this, provIface);

          if (replace) {
            m_requiredInterfaceMeta->attachInterface(m_requirerInstance->getInstanceObject(), replace->getProvidedInterfaceObject());
            m_requiredInterfaceMeta->detachInterface(m_requirerInstance->getInstanceObject(), providedInterface->getProvidedInterfaceObject());
            this->m_requirerInstance->checkDeactivate();
          }
          else {
            this->m_requirerInstance->checkDeactivate();
            m_requiredInterfaceMeta->detachInterface(m_requirerInstance->getInstanceObject(), providedInterface->getProvidedInterfaceObject());
          }
        }
        else {
          if (Optionality::MANDATORY == m_requiredInterfaceMeta->getOptionality() && Cardinality::SINGLE == m_requiredInterfaceMeta->getCardinality()) {
            this->m_requirerInstance->checkDeactivate();
            m_requiredInterfaceMeta->detachInterface(m_requirerInstance->getInstanceObject(), providedInterface->getProvidedInterfaceObject());
          }
          else {
            this->m_requirerInstance->checkDeactivate();
            m_requiredInterfaceMeta->detachInterface(m_requirerInstance->getInstanceObject(), providedInterface->getProvidedInterfaceObject());
          }
        }
        break;
      }
    }
    TRC_FUNCTION_LEAVE("");
  }

  void RequiredInterface::detachAllProvidedInterfaces()
  {
    TRC_FUNCTION_ENTER("");
    for (auto providedInterface : m_providedInterfaces) {
      m_requiredInterfaceMeta->detachInterface(m_requirerInstance->getInstanceObject(), providedInterface->getProvidedInterfaceObject());
    }
    TRC_FUNCTION_LEAVE("");
  }

  void RequiredInterface::attachProvidedInterface(const std::shared_ptr<ProvidedInterface>& providedInterface)
  {
    TRC_FUNCTION_ENTER(PAR(*providedInterface));

    if (m_providedInterfaces.empty() || Cardinality::MULTIPLE == m_requiredInterfaceMeta->getCardinality()) {
      m_providedInterfaces.push_back(providedInterface);
      m_requiredInterfaceMeta->attachInterface(m_requirerInstance->getInstanceObject(), providedInterface->getProvidedInterfaceObject());
      this->m_requirerInstance->checkActivate();
    }
    TRC_FUNCTION_LEAVE("");
  }

  bool RequiredInterface::operator == (const RequiredInterface& other) const
  {
    return m_requirerInstance->getInstanceObject() == other.m_requirerInstance->getInstanceObject();
  }

  bool RequiredInterface::isSatisfied() const
  {
    return !m_providedInterfaces.empty() || Optionality::UNREQUIRED == m_requiredInterfaceMeta->getOptionality();
  }

  std::ostream& operator<< (std::ostream& o, const RequiredInterface& ri)
  {
    o <<
      NAME_PAR(RequiredInterface, ri.m_requiredInterfaceMeta->getInterfaceName()) <<
      NAME_PAR(RequirerInstance, ri.m_requirerInstance->getInstanceName());
    return o;
  }
  
}
