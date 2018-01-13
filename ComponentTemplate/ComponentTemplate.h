#pragma once

#include "ITemplateService.h"
#include "ShapeProperties.h"
#include <string>

namespace shape {
  class ITraceService;

  class ComponentTemplate : public ITemplateService
  {
  public:
    ComponentTemplate();
    virtual ~ComponentTemplate();
    std::string doService(const std::string & str) const override;

    void activate(const Properties *props = 0);
    void deactivate();
    void modify(const Properties *props);

    void attachInterface(ITraceService* iface);
    void detachInterface(ITraceService* iface);

  private:
  };
}
