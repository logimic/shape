#pragma once

#include "ShapeDefines.h"
#include <string>

#ifdef ITemplateService_EXPORTS
#define ITemplateService_DECLSPEC SHAPE_ABI_EXPORT
#else
#define ITemplateService_DECLSPEC SHAPE_ABI_IMPORT
#endif

namespace shapeExpl {
  class ITemplateService_DECLSPEC ITemplateService
  {
  public:
    virtual std::string doService(const std::string & str) const = 0;
    virtual ~ITemplateService() {}
  };
}
