#define ComponentService_EXPORTS

#include "LauncherService.h"
#include "ComponentManager.h"
#include "ShapeComponent.h"
#include "StaticComponentMap.h"
#include "Component.h"
#include "Trace.h"

extern "C" {
  SHAPE_ABI_EXPORT void* get_component_shape__LauncherService(unsigned long* compiler, unsigned long* typehash)
  {
    static shape::ComponentMetaTemplate<shape::LauncherService> component("shape::LauncherService");
    component.provideInterface<shape::ILaunchService>("shape::ILaunchService");

    component.requireInterface<shape::ITraceService>("shape::ITraceService", shape::Optionality::UNREQUIRED, shape::Cardinality::MULTIPLE);

    *compiler = SHAPE_PREDEF_COMPILER;
    *typehash = std::type_index(typeid(shape::ComponentMeta)).hash_code();
    return &component;
  }

}

using namespace shape;
using namespace std;

namespace shape {
  void LauncherService::activate(const Properties *props)
  {
    TRC_FUNCTION_ENTER("");
    TRC_INFORMATION("\n******************************\n" << "ComponentLauncher::activate()\n" << "******************************");
    TRC_FUNCTION_LEAVE("")
  }

  void LauncherService::deactivate()
  {
    TRC_FUNCTION_ENTER("");
    TRC_INFORMATION("*\n*****************************\n" << "ComponentLauncher::deactivate()\n" << "******************************");
    TRC_FUNCTION_LEAVE("")
  }

  void LauncherService::modify(const Properties *props)
  {
  }

  void LauncherService::attachInterface(ITraceService* iface)
  {
    TRC_FUNCTION_ENTER(PAR(iface));
    shape::Tracer::get().addTracerService(iface);
    shape::Tracer::get().resetBuffered();
    shape::Tracer::get().writeBuffer();
    TRC_FUNCTION_LEAVE("")
  }

  void LauncherService::detachInterface(ITraceService* iface)
  {
    shape::Tracer::get().removeTracerService(iface);
  }

  //for ILaunchService
  const std::vector<std::string>& LauncherService::getCommandLine() const
  {
    return ComponentManager::getInst().getCommandLine();
  }

  const std::string& LauncherService::getDeploymentDir() const
  {
    return ComponentManager::getInst().getDeploymentDir();
  }

  const std::string& LauncherService::getConfigurationDir() const
  {
    return ComponentManager::getInst().getConfigurationDir();
  }

  const std::string& LauncherService::getResourceDir() const
  {
    return ComponentManager::getInst().getResourceDir();
  }

  const std::string& LauncherService::getDataDir() const
  {
    return ComponentManager::getInst().getDataDir();
  }

  const std::string& LauncherService::getCacheDir() const
  {
    return ComponentManager::getInst().getCacheDir();
  }

  const std::string& LauncherService::getUserDir() const
  {
    return ComponentManager::getInst().getUserDir();
  }

  const std::string& LauncherService::getAppName() const
  {
    return ComponentManager::getInst().getApplicationName();
  }

  std::string LauncherService::componentOpen(int id)
  {
    return ComponentManager::getInst().componentOpen(id);
  }

  std::string LauncherService::componentOpenByName(const std::string& componentName, const std::string& libraryPath, const std::string& libraryName)
  {
    return ComponentManager::getInst().componentOpenByName(componentName, libraryPath, libraryName);
  }

  std::string LauncherService::componentStart(int id)
  {
    return ComponentManager::getInst().componentStart(id);
  }

  std::string LauncherService::componentStop(int id)
  {
    return ComponentManager::getInst().componentStop(id);
  }

  std::string LauncherService::componentUnload(int id)
  {
    return ComponentManager::getInst().componentUnload(id);
  }

  void LauncherService::exit()
  {
    return ComponentManager::getInst().exit();
  }
  void LauncherService::exit(bool wait)
  {
    return ComponentManager::getInst().exit(wait);
  }
  bool LauncherService::isExiting() const
  {
    return ComponentManager::getInst().isExiting();
  }
  void LauncherService::upToRunning()
  {
    return ComponentManager::getInst().upToRunning();
  }

}