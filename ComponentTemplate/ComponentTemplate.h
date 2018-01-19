#pragma once

#include "ITemplateService.h"
#include "ShapeProperties.h"
#include "ITraceService.h"
#include <string>

namespace shapeExpl {
  class ComponentTemplate : public ITemplateService
  {
  public:
    ComponentTemplate();
    virtual ~ComponentTemplate();
    std::string doService(const std::string & str) const override;

    void activate(const shape::Properties *props = 0);
    void deactivate();
    void modify(const shape::Properties *props);

    void attachInterface(shape::ITraceService* iface);
    void detachInterface(shape::ITraceService* iface);

  private:
  };
}
