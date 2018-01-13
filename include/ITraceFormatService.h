#pragma once

#include <string>

#include "ITraceService.h"
#include "ShapeDefines.h"

#ifdef ITraceFromatService_EXPORTS
#define ITRACE_FORMAT_SERVICE_DECLSPEC SHAPE_ABI_EXPORT
#else
#define ITRACE_FORMAT_SERVICE_DECLSPEC SHAPE_ABI_IMPORT
#endif

namespace shape {
  class ITRACE_FORMAT_SERVICE_DECLSPEC ITraceFormatService
  {
  public:
    virtual std::string format(int level, int channel, const char* moduleName,
      const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg) const = 0;
    virtual ~ITraceFormatService() {}
  };
}
