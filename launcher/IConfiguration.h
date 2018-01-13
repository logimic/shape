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
    virtual void update() = 0;
    virtual void remove() = 0;
    virtual ~IConfiguration() {}
  };
}
