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

#define configurationService_EXPORTS

#include "ConfigurationService.h"
#include "ConfigurationManager.h"
#include "ComponentManager.h"
#include "ShapeComponent.h"
#include "StaticComponentMap.h"
#include "Component.h"
#include "Trace.h"

extern "C" {
  SHAPE_ABI_EXPORT void* get_component_shape__ConfigurationService(unsigned long* compiler, size_t* typehash)
  {
    static shape::ComponentMetaTemplate<shape::ConfigurationService> component("shape::ConfigurationService");
    component.provideInterface<shape::IConfigurationService>("shape::IConfigurationService");

    component.requireInterface<shape::ITraceService>("shape::ITraceService", shape::Optionality::UNREQUIRED, shape::Cardinality::MULTIPLE);

    *compiler = SHAPE_PREDEF_COMPILER;
    *typehash = std::type_index(typeid(shape::ComponentMeta)).hash_code();
    return &component;
  }

}

using namespace shape;
using namespace std;

namespace shape {
  //for IConfigurationService
  IConfiguration* ConfigurationService::createConfiguration(const std::string& componentName, const std::string& instanceName)
  {
    return ComponentManager::getInst().getConfigurationMgr().createConfiguration(componentName, instanceName);
  }

  IConfiguration* ConfigurationService::getConfiguration(const std::string& componentName, const std::string& instanceName)
  {
    return ComponentManager::getInst().getConfigurationMgr().getConfiguration(componentName, instanceName);
  }

  std::vector<shape::IConfiguration*> ConfigurationService::listConfigurations(const std::string& componentName)
  {
    return ComponentManager::getInst().getConfigurationMgr().listConfigurations(componentName);
  }

  void ConfigurationService::activate(const Properties *props)
  {
    (void)props; //silence -Wunused-parameter
    TRC_FUNCTION_ENTER("");
    TRC_INFORMATION("\n******************************\n" << "ConfigurationService::activate()\n" << "******************************");
    TRC_FUNCTION_LEAVE("")
  }

  void ConfigurationService::deactivate()
  {
    TRC_FUNCTION_ENTER("");
    TRC_INFORMATION("*\n*****************************\n" << "ConfigurationService::deactivate()\n" << "******************************");
    TRC_FUNCTION_LEAVE("")
  }

  void ConfigurationService::modify(const Properties *props)
  {
    (void)props; //silence -Wunused-parameter
  }

  void ConfigurationService::attachInterface(ITraceService* iface)
  {
    //TRC_FUNCTION_ENTER(PAR(iface));
    //shape::Tracer::get().addTracerService(iface);
    //shape::Tracer::get().resetBuffered();
    //shape::Tracer::get().writeBuffer();
    //TRC_FUNCTION_LEAVE("")
  }

  void ConfigurationService::detachInterface(ITraceService* iface)
  {
    shape::Tracer::get().removeTracerService(iface);
  }

}
