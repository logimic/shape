#pragma once

#include "ITraceService.h"
#include "ITraceFormatService.h"
#include "ShapeProperties.h"
#include <string>

namespace shape {

  class TraceFileService : public ITraceService
  {
  public:
    TraceFileService();
    virtual ~TraceFileService();

    bool isValid(int level, int chan) const override;
    void writeMsg(int level, int channel, const char* moduleName,
      const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg) override;
    
    void activate(const Properties *props = 0);
    void deactivate();
    void modify(const Properties *props);

    void attachInterface(ITraceFormatService* iface);
    void detachInterface(ITraceFormatService* iface);

  private:
    class Imp;
    Imp * imp;
  };
}
