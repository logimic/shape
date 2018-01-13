#include "ConfigurationManager.h"
#include "Configuration.h"
#include "ShapeDefines.h"
#include "ShapePropertiesJson.h"
#include "ShapePropertiesMacros.h"

#include "Trace.h"

#include <functional>

using namespace std;
using namespace shape;

namespace shape {

  const std::string& Configuration::getComponentName() const
  {
    return m_componentName;
  }

  const std::string& Configuration::getId() const
  {
    return m_id;
  }

  std::shared_ptr<Properties> & Configuration::getProperties()
  {
    return m_properties;
  }

  Configuration::Configuration(const std::string& id, const std::string& componentName, ConfigurationManager* cmgr)
    : m_id(id)
    , m_componentName(componentName)
    , m_cmgr(cmgr)
  {
    m_properties.reset(shape_new shape::PropertiesJson());
    //add mandatory props
    try {
      SET_MEMBER_AS(*m_properties, String, CONFIG_INSTANCE_STR, "", m_id);
    }
    catch (...) {
      THROW_EXC_TRC_WAR(std::logic_error, "Cannot add mandatory property: " << PAR(CONFIG_INSTANCE_STR));
    }

    try {
      SET_MEMBER_AS(*m_properties, String, CONFIG_COMPONENT_STR, "", m_componentName);
    }
    catch (...) {
      THROW_EXC_TRC_WAR(std::logic_error, "Cannot add mandatory property: " << PAR(CONFIG_COMPONENT_STR));
    }

  }

  Configuration::Configuration(const std::string& fname, ConfigurationManager* cfgAdmin)
    : m_id("")
    , m_componentName("")
    , m_properties(0)
    , m_filename(fname)
    , m_cmgr(cfgAdmin)
  {
    parse(m_filename);
  }

  Configuration::~Configuration()
  {
  }

  void Configuration::update()
  {
    TRC_FUNCTION_ENTER("");
    m_cmgr->update(this);
    TRC_FUNCTION_LEAVE("");
  }

  void Configuration::remove()
  {
    TRC_FUNCTION_ENTER("");
    m_cmgr->remove(this);
    TRC_FUNCTION_LEAVE("");
  }

  void Configuration::parse(const std::string& fname)
  {
    TRC_FUNCTION_ENTER(PAR(fname));
    m_properties.reset(shape_new shape::PropertiesJson());
    try {
      m_properties->parseFile(fname);
    }
    catch (std::exception &e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "When parsing: " << PAR(fname) << " rethrown");
      throw e;
    }

    try {
      GET_MEMBER_AS(*m_properties, String, CONFIG_INSTANCE_STR, "", m_id);
    }
    catch (std::exception & e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "Missing mandatory property: " << PAR(CONFIG_INSTANCE_STR) <<
        " when parsing: " << PAR(fname));
    }

    try {
      GET_MEMBER_AS(*m_properties, String, CONFIG_COMPONENT_STR, "", m_componentName);
    }
    catch (std::exception & e) {
      CATCH_EXC_TRC_WAR(std::exception, e, "Missing mandatory property: " << PAR(CONFIG_COMPONENT_STR) <<
        " when parsing: " << PAR(fname));
    }
    TRC_FUNCTION_ENTER("");
  }

  void Configuration::encode(const string& fname)
  {
    if (m_properties)
      m_properties->encodeFile(fname);
  }

}
