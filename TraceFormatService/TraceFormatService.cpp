#define ITraceFromatService_EXPORTS

#include "TraceFormatService.h"
#include "TraceLevel.h"
#include "ShapePropertiesMacros.h"
#include "Trace.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <time.h>

#include "shape__TraceFormatService.hxx"

TRC_INIT_MODULE(shape::TraceFormatService);

using namespace std;

namespace shape {

  class TraceFormatService::Impl {
  public:

    Impl()
    {
    }

    ~Impl()
    {
    }

    std::string format(int level, int channel, const char* moduleName,
      const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg) const
    {
      ostringstream o;

      // write time
      using namespace std::chrono;
      auto nowTimePoint = std::chrono::system_clock::now();
      auto nowTimePointUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTimePoint.time_since_epoch()).count() % 1000000;
      auto time = std::chrono::system_clock::to_time_t(nowTimePoint);
      auto tm = *std::localtime(&time);
      char buf[80];
      strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", &tm);
      
      // remove path from source file
      std::string fname = sourceFile;
      size_t found = fname.find_last_of("/\\");
      if (std::string::npos != found)
        fname = fname.substr(found + 1);

      // format msg
      o << std::setfill('0') << std::setw(6) << buf << '.' << nowTimePointUs << ' ' <<
        TraceLevelStringConvertor::int2str(level) << ' ' << channel << ' ' <<
        moduleName << ' ' << fname << ':' << sourceLine << ' ' << funcName << "() " << std::endl << 
        msg;
      o.flush();

      return o.str();

    }

    void modify(const Properties *props)
    {
    }

  };

  //////////////////////////////////////////////////////////////////////////
  TraceFormatService::TraceFormatService()
  {
    impl = shape_new Impl();
  }

  TraceFormatService::~TraceFormatService()
  {
    delete impl;
  }

  std::string TraceFormatService::format(int level, int channel, const char* moduleName,
    const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg) const
  {
    return impl->format(level, channel, moduleName, sourceFile, sourceLine, funcName, msg);
  }

  void TraceFormatService::activate(const Properties *props)
  {
    impl->modify(props);
  }

  void TraceFormatService::deactivate()
  {
  }

  void TraceFormatService::modify(const Properties *props)
  {
    impl->modify(props);
  }

}