#define ComponentService_EXPORTS

#include "Component.h"
#include "ComponentManager.h"
#include "ComponentInstance.h"
#include "ProvidedInterface.h"
#include "RequiredInterface.h"
#include "Trace.h"
#include "ShapePropertiesMacros.h"
#include "ShapeDefines.h"

#include <iostream>
#include <array>

using namespace std;
using namespace shape;

namespace shape
{
  ComponentManager& ComponentManager::getInst() {
    static ComponentManager s;
    return s;
  }

  ComponentManager::ComponentManager()
    : m_configurationDir("./configuration")
    , m_deploymentDir("./configuration")
    , m_applicationName("Shape startup")
    , m_predefinedCompiler(SHAPE_PREDEF_COMPILER)
  {
  }

  ComponentManager::~ComponentManager()
  {
    eraseComponents();
  }

  string ComponentManager::componentOpenByName(
    const std::string& componentName,
    const std::string& libraryPath,
    const std::string& libraryName)
  {
    TRC_FUNCTION_ENTER(PAR(componentName));
    ostringstream ostr;

    try {
      shared_ptr<Component> comp = createComponent(0, componentName, libraryPath, libraryName);
      ostr << "Created component: " << PAR(componentName);
    }
    catch (std::exception & e) {
      CATCH_EXC_TRC_WAR(std::exception, e, NAME_PAR(deploymentDir, m_deploymentDir));
      ostr << "Component: " << PAR(componentName) << PAR(libraryPath) << PAR(libraryName) << "doesn't exist";
    }

    TRC_FUNCTION_LEAVE(PAR(componentName));
    return ostr.str();
  }

  string ComponentManager::componentOpen(int id)
  {
    TRC_FUNCTION_ENTER(PAR(id));
    ostringstream ostr;

    lock_guard<recursive_mutex> lck(m_componentMtx);

    auto found = m_indexComponents.find(id);
    if (found != m_indexComponents.end()) {
      Component& cmp = *found->second.get();
      if (Component::MyState::CLOSED == cmp.getState()) {
        cmp.openLibrary();
      }
      else {
        ostr << PAR(id) << NAME_PAR(componentName, cmp.getComponentName()) " already: " << cmp.getStateStr() << endl;
      }
    }
    else {
      ostr << PAR(id) << " is not valid Component id" << endl;
    }

    TRC_FUNCTION_LEAVE(PAR(id));
    return ostr.str();
  }

  string ComponentManager::componentStart(int id)
  {
    TRC_FUNCTION_ENTER(PAR(id));
    ostringstream ostr;

    lock_guard<recursive_mutex> lck(m_componentMtx);

    auto found = m_indexComponents.find(id);
    if (found != m_indexComponents.end()) {
      Component& cmp = *found->second.get();
      if (Component::MyState::STARTED != cmp.getState()) {
        cmp.startInstances();
      }
      else {
        ostr << PAR(id) << NAME_PAR(componentName, cmp.getComponentName()) " already: " << cmp.getStateStr() << endl;
      }
    }
    else {
      ostr << PAR(id) << " is not valid Component id" << endl;
    }

    TRC_FUNCTION_LEAVE(PAR(id));
    return ostr.str();
  }

  string ComponentManager::componentStop(int id)
  {
    TRC_FUNCTION_ENTER(PAR(id));
    TRC_INFORMATION("Stopping component: " << PAR(id));
    std::ostringstream ostr;

    lock_guard<recursive_mutex> lck(m_componentMtx);

    auto found = m_indexComponents.find(id);
    if (found != m_indexComponents.end()) {
      Component& cmp = *found->second.get();
      if (Component::MyState::STARTED == cmp.getState()) {
        cmp.stopInstances();
      }
      else {
        ostr << PAR(id) << NAME_PAR(componentName, cmp.getComponentName()) " already: " << cmp.getStateStr() << endl;
      }
    }
    else {
      ostr << PAR(id) << " is not valid Component id" << endl;
    }

    TRC_FUNCTION_LEAVE("");
    return ostr.str();
  }

  string ComponentManager::componentUnload(int id)
  {
    TRC_FUNCTION_ENTER(PAR(id));
    TRC_INFORMATION("Unload component: " << PAR(id));

    std::ostringstream ostr;

    lock_guard<recursive_mutex> lck(m_componentMtx);

    auto found = m_indexComponents.find(id);
    if (found != m_indexComponents.end()) {
      Component& cmp = *found->second.get();
      if (Component::MyState::STARTED == cmp.getState() || Component::MyState::OPENED == cmp.getState()) {
        cmp.closeLibrary();
      }
      else {
        ostr << PAR(id) << NAME_PAR(componentName, cmp.getComponentName()) " already: " << cmp.getStateStr() << endl;
      }
    }
    else {
      ostr << PAR(id) << " is not valid Component id" << endl;
    }

    TRC_FUNCTION_LEAVE(PAR(id));
    return ostr.str();
  }

  void ComponentManager::exit()
  {
    TRC_FUNCTION_ENTER("");
    exit(false);
    TRC_FUNCTION_LEAVE("");
  }

  void ComponentManager::exit(bool wait)
  {
    TRC_FUNCTION_ENTER("");
    {
      lock_guard<mutex> lock(m_exitMtx);
      m_exit = true;
    }
    m_exitCond.notify_all();

    if (wait) {
      unique_lock<mutex> lock(m_exitMtx);
      m_exitCond.wait(lock, [this](){ return m_exitComplete; });
    }
    TRC_FUNCTION_LEAVE("");
  }

  bool ComponentManager::isExiting() const
  {
    lock_guard<mutex> lock(m_exitMtx);
    return m_exit;
  }

  std::shared_ptr<Component> ComponentManager::createComponent(
    int startLvl,
    const std::string& componentName,
    const std::string& libraryPath,
    const std::string& libraryName)
  {
    TRC_FUNCTION_ENTER(PAR(componentName) << PAR(libraryPath) << PAR(libraryName) << PAR(startLvl));
    TRC_INFORMATION("Creating component: " << PAR(componentName));

    static int autoComponentIndex = 16;

    lock_guard<recursive_mutex> lck(m_componentMtx);
    std::shared_ptr<Component> cmpPtr;

    auto found = m_nameComponents.find(componentName);
    if (found != m_nameComponents.end()) {
      TRC_INFORMATION("Component already created: " << PAR(componentName));
      cmpPtr = found->second;
    }
    else {
      cmpPtr.reset(shape_new Component(componentName, libraryPath, libraryName, m_deploymentDir));

      ++autoComponentIndex;

      auto result = m_indexComponents.insert(make_pair(autoComponentIndex, cmpPtr));
      if (!result.second) {
        THROW_EXC_TRC_WAR(std::logic_error, "Wrong index: " << PAR(autoComponentIndex) << PAR(componentName));
      }

      m_nameComponents.insert(make_pair(componentName, cmpPtr));

      //remove from disabled list
      auto found = m_disabledComponents.find(cmpPtr->getComponentName());
      if (found != m_disabledComponents.end())
        m_disabledComponents.erase(found);
    }

    TRC_FUNCTION_LEAVE(PAR(componentName));
    return cmpPtr;
  }

  void ComponentManager::startComponents()
  {
    TRC_FUNCTION_ENTER("");

    lock_guard<recursive_mutex> lck(m_componentMtx);

    for (auto cmp : m_indexComponents) {
      cmp.second->startInstances();
    }

    //TODO test cfg update
    /*
    IConfiguration* cfg = getConfigurationMgr().getConfiguration("TraceFileService","TraceFileService-std");
    cfg->update();
    cfg->remove();
    cfg = getConfigurationMgr().createConfiguration("TraceFileService", "dummy", false);
    cfg->update();
    */

    TRC_FUNCTION_LEAVE("");
  }

  void ComponentManager::eraseComponents()
  {
    TRC_FUNCTION_ENTER("");

    lock_guard<recursive_mutex> lck(m_componentMtx);

    try {
      //iterate in reverse order to keep start level
      while (!m_indexComponents.empty()) {
        auto rvit = m_indexComponents.rbegin();
        rvit->second->stopInstances();
        rvit->second->closeLibrary();
        rvit++;
        m_indexComponents.erase(rvit.base());
      }
    }
    catch (std::exception &e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "");
    }

    TRC_FUNCTION_LEAVE("");
  }

  void ComponentManager::addProvidedInterface(std::shared_ptr<ProvidedInterface> providedInterface)
  {
    TRC_FUNCTION_ENTER(PAR(*providedInterface));

    TRC_INFORMATION("Add provided interface: " << providedInterface->getProvidedInterfaceObject()->getName());

    try {
      m_providedInterfaces.insert(make_pair(providedInterface->getProvidedInterfaceObject()->getTypeIndex(), providedInterface));

      std::vector<std::shared_ptr<RequiredInterface>> reqIfcVect;

      //find waiting required interfaces
      auto range = m_requiredInterfaces.equal_range(providedInterface->getProvidedInterfaceObject()->getTypeIndex());
      for (auto reqIfc = range.first; reqIfc != range.second; ++reqIfc) {
        if (providedInterface->isTarget(reqIfc->second->getTargetProps()))
          reqIfcVect.push_back(reqIfc->second);
      }

      //separate iterator handling
      for (auto serRef : reqIfcVect) {
        serRef->attachProvidedInterface(providedInterface);
      }

    }
    catch (std::exception & e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "Cannot register servis - caught and rethrown");
      throw e;
    }

    TRC_FUNCTION_LEAVE("");
  }

  void ComponentManager::removeProvidedInterface(std::shared_ptr<ProvidedInterface> providedInterface)
  {
    TRC_FUNCTION_ENTER(PAR(*providedInterface));

    if (0 < m_providedInterfaces.size()) {
      auto erangep = m_providedInterfaces.equal_range(providedInterface->getProvidedInterfaceObject()->getTypeIndex());
      for (auto prvit = erangep.first; prvit != erangep.second;) {
        if (*prvit->second != *providedInterface) {
          ++prvit;
        }
        else {
          TRC_INFORMATION("Remove provided interface: " << prvit->second->getProvidedInterfaceObject()->getName());

          std::vector<std::shared_ptr<RequiredInterface>> reqIfcVect;

          auto eranger = m_requiredInterfaces.equal_range(providedInterface->getProvidedInterfaceObject()->getTypeIndex());
          for (auto req = eranger.first; req != eranger.second; ++req) {
            if (providedInterface->isTarget(req->second->getTargetProps()))
                reqIfcVect.push_back(req->second);
          }

          //separate iterator handling
          for (auto reqIfc : reqIfcVect) {
            reqIfc->detachProvidedInterface(providedInterface);
          }

          //refresh
          auto erangep2 = m_providedInterfaces.equal_range(providedInterface->getProvidedInterfaceObject()->getTypeIndex());
          for (auto it2 = erangep2.first; it2 != erangep2.second; it2++) {
            if (*it2->second == *providedInterface) { //the only one selected => break
              m_providedInterfaces.erase(it2);
              break;
            }
          }
          break;
        }
      }
    }

    TRC_FUNCTION_LEAVE("");
  }

  std::shared_ptr<ProvidedInterface> ComponentManager::getReplacingProvidedInterface(
    const RequiredInterface& requiredInterface, std::shared_ptr<ProvidedInterface>& currentProvidedInterface)
  {
    TRC_FUNCTION_ENTER(PAR(requiredInterface) << PAR(*currentProvidedInterface));

    std::shared_ptr<ProvidedInterface> result;

    auto range = m_providedInterfaces.equal_range(requiredInterface.getRequiredIntefaceMeta()->getInterfaceTypeIndex());
    for (auto provIfaceIt = range.first; provIfaceIt != range.second;)
    {
      ProvidedInterface& providedInterface = *(provIfaceIt->second);
      if (!(*(provIfaceIt->second) == *currentProvidedInterface)) {
        if (providedInterface.isTarget(requiredInterface.getTargetProps())) {
          result = provIfaceIt->second;
        }
      }
    }

    if (result) {
      TRC_DEBUG("replacing: " << NAME_PAR(providedInterface, *result));
    }
    else {
      TRC_DEBUG("not found replacing service: ");
    }

    TRC_FUNCTION_ENTER(PAR(result));
    return result;
  }

  void ComponentManager::addRequiredInterface(std::shared_ptr<RequiredInterface> requiredInterface)
  {
    TRC_FUNCTION_ENTER(PAR(*requiredInterface));

    try {
      const type_index &tix = requiredInterface->getRequiredIntefaceMeta()->getInterfaceTypeIndex();
      m_requiredInterfaces.insert(make_pair(tix, requiredInterface));

      auto range = m_providedInterfaces.equal_range(tix);
      for (auto provIface = range.first; provIface != range.second; ++provIface) {
        if (provIface->second->isTarget(requiredInterface->getTargetProps()))

          requiredInterface->attachProvidedInterface(provIface->second);
      }
    }
    catch (std::exception &e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "Cannot reference servis - caught and rethrown");
      throw e;
    }

    TRC_FUNCTION_LEAVE("");
  }

  void ComponentManager::removeRequiredInterface(std::shared_ptr<RequiredInterface> requiredInterface)
  {
    TRC_FUNCTION_ENTER(PAR(*requiredInterface));
    auto erg = m_requiredInterfaces.equal_range(requiredInterface->getRequiredIntefaceMeta()->getInterfaceTypeIndex());
    for (auto req = erg.first; req != erg.second;) {
      if (*req->second == *requiredInterface) {
        m_requiredInterfaces.erase(req++);
      }
      else {
        ++req;
      }
    }
    TRC_FUNCTION_LEAVE("");
  }

  int ComponentManager::init(const std::vector<std::string>& commandLine, PropertiesJson* props)
  {
    cout <<
      "================================================================" << endl <<
      "                     Shape.it" << endl <<
      "================================================================" << endl <<
      "Copyright 2018 Logimic.com" << endl << endl;

    TRC_FUNCTION_ENTER(NAME_PAR(time, time(nullptr)));

    // save delivered cmdl parameters
    m_commandLine = commandLine;

    // check delivered props
    if (props != nullptr) {
      m_properties = *props;
    }
    else {
      // properties taken as 2.st cmdl param (1.st is exe)
      if (m_commandLine.size() > 1) {
        cout << "Launcher: Loading configuration file: " << m_commandLine[1] << endl;
        try {
          m_properties.parseFile(m_commandLine[1]);
        }
        catch (std::exception & e) {
          cerr << "Launcher: Caught exception: " << e.what() << endl;
        }
      }
      else {
        cout << "Launcher: Missing configuration file" << endl;
        return -1;
      }
    }

    //cout << "Launcher: Started ..." << endl;


    m_deploymentDir = "configuration";
    m_configurationDir = "configuration";
    m_resourceDir = "./";
    m_dataDir = "./";
    m_cacheDir = "./";
    m_userDir = "./";

    try {

      GET_POSSIBLE_MEMBER_AS(m_properties, String, "applicationName", "", m_applicationName);
      GET_POSSIBLE_MEMBER_AS(m_properties, String, "resourceDir", "", m_resourceDir);
      GET_POSSIBLE_MEMBER_AS(m_properties, String, "dataDir", "", m_dataDir);
      GET_POSSIBLE_MEMBER_AS(m_properties, String, "cacheDir", "", m_cacheDir);
      GET_POSSIBLE_MEMBER_AS(m_properties, String, "userDir", "", m_userDir);
      GET_POSSIBLE_MEMBER_AS(m_properties, String, "configurationDir", "", m_configurationDir);
      GET_POSSIBLE_MEMBER_AS(m_properties, String, "deploymentDir", "", m_deploymentDir);
      
      m_configurationDir = m_resourceDir + m_configurationDir;
      cout << "Configuration directory set to: " << m_configurationDir << endl;
      m_deploymentDir = m_resourceDir + m_deploymentDir;

      TRC_INFORMATION(PAR(m_applicationName));
      TRC_INFORMATION(PAR(m_resourceDir));
      TRC_INFORMATION(PAR(m_dataDir));
      TRC_INFORMATION(PAR(m_cacheDir));
      TRC_INFORMATION(PAR(m_userDir));
      TRC_INFORMATION(PAR(m_configurationDir));
      TRC_INFORMATION(PAR(m_deploymentDir));

      m_configurationMgr.setCfgDir(m_configurationDir);
      m_configurationMgr.loadExistingConfigurations();

      std::vector<std::unique_ptr<Properties>> compVect;
      GET_MEMBER_AS_VECTOR(m_properties, Properties, "components", "", compVect);

      int i = 0;
      std::multimap<int, array<std::string,3>> startOrdering;

      for (auto & comp : compVect)
      {
        string componentName;
        GET_MEMBER_AS(*comp, String, "name", "components[" << i << "]", componentName);
        string libraryPath;
        GET_POSSIBLE_MEMBER_AS(*comp, String, "libraryPath", "components[" << i << "]", libraryPath);
        string libraryName;
        GET_POSSIBLE_MEMBER_AS(*comp, String, "libraryName", "components[" << i << "]", libraryName);
        bool ebl = true;
        GET_POSSIBLE_MEMBER_AS(*comp, Bool, "enabled", "components[" << i << "]", ebl);
        int startLevel = 100;
        GET_POSSIBLE_MEMBER_AS(*comp, Int, "startlevel", "components[" << i << "]", startLevel);

        if (ebl) {
          array<std::string, 3> nams = { componentName, libraryPath, libraryName };
          startOrdering.insert(make_pair(startLevel, nams));
        }
        else
          m_disabledComponents.insert(componentName);
      }

      for (auto it : startOrdering) {
        createComponent(it.first, it.second[0], it.second[1], it.second[2]);
      }

    }
    catch (...) {
      eraseComponents();
      throw;
    }

    TRC_FUNCTION_LEAVE("");
    return 0;
  }

  void ComponentManager::upToRunning()
  { 
    TRC_FUNCTION_ENTER("");
    if (this_thread::get_id() == m_myThreadId)
    {
      THROW_EXC_TRC_WAR(std::logic_error, "upToRunning called from main thread");
    }
    std::unique_lock<std::mutex> lck(m_runningMutex);
    while (!m_running) m_runningCondition.wait(lck);
    TRC_FUNCTION_LEAVE("");
  }

  void ComponentManager::run()
  {
    try {
      m_myThreadId = this_thread::get_id();
      startComponents();

      {
        std::lock_guard<std::mutex> lck(m_runningMutex);
        m_running = true;
        m_runningCondition.notify_all();
      }

      {
        unique_lock<mutex> lock(m_exitMtx);
        m_exitCond.wait(lock, [this]() { return m_exit; });
      }

      eraseComponents();

      {
        lock_guard<mutex> lock(m_exitMtx);
        m_exitComplete = true;
      }
      m_exitCond.notify_all();
    }
    catch (exception &e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "Stopped");
      cerr << "Stopped: caught exception: " << e.what() << endl;
    }
    catch (...) {
      TRC_WARNING("Stopped: caught exception: " << "Unknown exception");
      cerr << "Stopped: caught exception: " << "Unknown exception" << endl;
    }
  }

}
