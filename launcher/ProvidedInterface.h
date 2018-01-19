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
