#pragma once

#include "ITraceFormatService.h"
#include "ShapeProperties.h"

namespace shape {
  class TraceFormatService: public ITraceFormatService
  {
  public:
    std::string format(int level, int channel, const char* moduleName,
      const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg) const override;

    void activate(const Properties *props = 0);
    void deactivate();
    void modify(const Properties *props);

    TraceFormatService();
    ~TraceFormatService();

  private:
    class Impl;
    Impl * impl;
  };
}
