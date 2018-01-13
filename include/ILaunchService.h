#pragma once

#include "ShapeDefines.h"
#include <vector>
#include <string>

#ifdef ComponentService_EXPORTS
#define COMPONENT_SERVICE_DECLSPEC SHAPE_ABI_EXPORT
#else
#define COMPONENT_SERVICE_DECLSPEC SHAPE_ABI_IMPORT
#endif

namespace shape {

  class COMPONENT_SERVICE_DECLSPEC ILaunchService
  {
  public:
    virtual const std::vector<std::string>& getCommandLine() const = 0;
    virtual const std::string& getDeploymentDir() const = 0;
    virtual const std::string& getConfigurationDir() const = 0;
    virtual const std::string& getResourceDir() const = 0;
    virtual const std::string& getDataDir() const = 0;
    virtual const std::string& getCacheDir() const = 0;
    virtual const std::string& getUserDir() const = 0;
    virtual const std::string& getAppName() const = 0;

    virtual std::string componentOpenByName(const std::string& componentName, const std::string& libraryPath, const std::string& libraryName) = 0;
    virtual std::string componentOpen(int id) = 0;
    virtual std::string componentStart(int id) = 0;
    virtual std::string componentStop(int id) = 0;
    virtual std::string componentUnload(int id) = 0;

    virtual void upToRunning() = 0;
    virtual void exit() = 0;
    virtual void exit(bool wait) = 0;
    virtual bool isExiting() const = 0;
    virtual ~ILaunchService() {}
  };
}
