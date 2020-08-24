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

#include "ShapePropertiesJson.h"
#include <string>

namespace shape {
  class IConfiguration
  {
  public:
    virtual const std::string& getComponentName() const = 0;
    virtual const std::string& getId() const = 0;
    virtual std::shared_ptr<Properties> & getProperties() = 0;
    virtual void update(bool persistent = false) = 0;
    virtual void remove() = 0;
    virtual ~IConfiguration() {}
  };
}
