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

#include "ConfigurationManager.h"
#include "Configuration.h"
#include "ShapeDefines.h"
#include "ShapePropertiesJson.h"
#include "ShapePropertiesMacros.h"
#include "rapidjson/schema.h"

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
    using namespace rapidjson;

    // set as object to allow members adding
    Document & doc = m_properties->getAsJson();
    doc.SetObject();

    //add mandatory props
    std::string inst = "/";
    inst += CONFIG_INSTANCE_STR;
    std::string comp = "/";
    comp += CONFIG_COMPONENT_STR;
    Pointer(inst).Set(doc, m_id, doc.GetAllocator());
    Pointer(comp).Set(doc, m_componentName, doc.GetAllocator());
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

  void Configuration::validate(const rapidjson::SchemaDocument& sd)
  {
    TRC_FUNCTION_ENTER("");
    
    using namespace rapidjson;

    m_validated = false;
    SchemaValidator validator(sd);
    if (!m_properties->getAsJson().Accept(validator)) {
      // Input JSON is invalid according to the schema
      StringBuffer sb;
      std::string schema, keyword, document;
      validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
      schema = sb.GetString();
      keyword = validator.GetInvalidSchemaKeyword();
      sb.Clear();
      validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
      document = sb.GetString();
      THROW_EXC_TRC_WAR(std::logic_error, "Invalid configuration: " <<
        PAR(schema) << PAR(keyword) << NAME_PAR(message, document));
    }
    m_validated = true;
    TRC_FUNCTION_LEAVE("")
  }
}
