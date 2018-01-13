#pragma once

#include "IConfiguration.h"
#include "ShapeDefines.h"
#include <string>

#ifdef configurationManager_EXPORTS
#define CONFIGURATIONMANAGER_DECLSPEC SHAPE_ABI_EXPORT
#else
#define CONFIGURATIONMANAGER_DECLSPEC SHAPE_ABI_IMPORT
#endif

namespace shape {
  class CONFIGURATIONMANAGER_DECLSPEC IConfigurationManager
  {
  public:
    virtual IConfiguration* createConfiguration(const std::string& componentName, const std::string& instanceName) = 0;
    virtual IConfiguration* getConfiguration(const std::string& componentName, const std::string& instanceName) = 0;
    virtual std::vector<shape::IConfiguration*> listConfigurations(const std::string& componentName) = 0;
    virtual ~IConfigurationManager() {}
  };
}
