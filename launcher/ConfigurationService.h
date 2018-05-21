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

#include "IConfigurationService.h"
#include "ITraceService.h"
#include "ShapeProperties.h"

namespace shape
{
  class ConfigurationService : public IConfigurationService
  {
  public:

    //for IConfigurationService
    IConfiguration* createConfiguration(const std::string& componentName, const std::string& instanceName) override;
    IConfiguration* getConfiguration(const std::string& componentName, const std::string& instanceName) override;
    std::vector<shape::IConfiguration*> listConfigurations(const std::string& componentName) override;

    void activate(const Properties *props = 0);
    void deactivate();
    void modify(const Properties *props);

    //TODO
    void attachInterface(shape::ITraceService* iface);
    void detachInterface(shape::ITraceService* iface);
  };
  
}
