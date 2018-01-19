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
