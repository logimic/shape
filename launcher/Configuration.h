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

#include "IConfiguration.h"
#include <memory>

namespace shape {
  class ConfigurationManager;

  class Configuration : public IConfiguration
  {
  public:
    const std::string& getComponentName() const override;
    const std::string& getId() const override;
    std::shared_ptr<Properties> & getProperties() override;
    void update() override;
    void remove() override;

    virtual ~Configuration();
    const std::string& getFilename() const { return m_filename; }
    bool equals(const IConfiguration& other) const { return other.getId() == m_id; }

  protected:
    friend class ConfigurationManager;

    Configuration(const std::string& id, const std::string& componentName, ConfigurationManager* cmgr);
    Configuration(const std::string& fname, ConfigurationManager* cfgAdmin);

    void parse(const std::string& fname);
    void encode(const std::string& fname);

    std::string m_id;
    std::string m_componentName;
    std::shared_ptr<Properties> m_properties;
    std::string m_filename;
    ConfigurationManager* m_cmgr = nullptr;
  private:
    Configuration();
  };
}
