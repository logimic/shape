#pragma once

#include "ShapeProperties.h"
#include <string>
#include <thread>

namespace shape {
  class ITraceService;

  class Example1_Thread
  {
  public:
    Example1_Thread();
    virtual ~Example1_Thread();

    void activate(const Properties *props = 0);
    void deactivate();
    void modify(const Properties *props);

    void attachInterface(ITraceService* iface);
    void detachInterface(ITraceService* iface);

  private:
    bool m_runTreadFlag = true;
    void runTread();
    std::thread m_thread;
  };
}
