#pragma once

#include <string>
#include <vector>

#include "ShapeDefines.h"

#ifdef ITraceService_EXPORTS
#define ITRACE_SERVICE_DECLSPEC SHAPE_ABI_EXPORT
#else
#define ITRACE_SERVICE_DECLSPEC SHAPE_ABI_IMPORT
#endif

namespace shape {
  class ITRACE_SERVICE_DECLSPEC ITraceService
  {
  public:
    virtual bool isValid(int level, int channel) const = 0;
    virtual void writeMsg(int level, int channel, const char* moduleName,
      const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg) = 0;
    virtual ~ITraceService() {}
  };
}
