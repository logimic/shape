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
