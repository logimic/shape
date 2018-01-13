#define ITraceService_EXPORTS
#include "TraceFileService.h"
#include "TraceLevel.h"
#include "ShapePropertiesMacros.h"
#include "ShapeDefines.h"
#include <fstream>
#include <iostream>
#include <mutex>

#ifdef SHAPE_PLATFORM_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "shape__TraceFileService.hxx"

TRC_INIT_MODULE(shape::TraceFileService);

namespace shape {
  const int FILE_SIZE_1MB = 0x100000;

  class TraceFileService::Imp
  {
  private:
    std::ofstream m_traceFile;
    std::string m_tracePath;
    std::string m_tracePathFile;
    long long m_maxFileSize = FILE_SIZE_1MB;
    std::mutex m_mtx;
    std::map<int, int> m_traceLevelMap;
    ITraceFormatService* m_formatService = nullptr;

  public:
    Imp()
    {
    }

    ~Imp()
    {
      closeFile();
    }

    bool isValid(int level, int chan) const
    {
      auto it = m_traceLevelMap.find(chan);
      return (it != m_traceLevelMap.end() && it->second >= level);
    }

    void writeMsg(int level, int channel, const char* moduleName,
      const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg)
    {
      std::lock_guard<std::mutex> guard(m_mtx);

      if (isValid(level, channel)) {
        openFile();

        // if file size is greater than m_maxFileSize, reset log file
        if (m_traceFile.tellp() > m_maxFileSize)
        {
          resetFile();
        }
        if (m_formatService)
          m_traceFile << m_formatService->format(level, channel, moduleName, sourceFile, sourceLine, funcName, msg);
        else
          //TODO better default format
          m_traceFile << (int)level << ':' << channel << " " << moduleName << msg;
      }
    }

    void modify(const Properties *props)
    {
      std::lock_guard<std::mutex> guard(m_mtx);

      if (props) {
        try {
          std::vector<std::unique_ptr<Properties>> vlvVect;
          GET_MEMBER_AS_VECTOR(*props, Properties, "VerbosityLevels", "", vlvVect);
          for (auto & vlv : vlvVect) {
            int channel;
            GET_MEMBER_AS(*vlv, Int, "channel", "", channel);
            std::string lvlStr;
            GET_MEMBER_AS(*vlv, String, "level", "", lvlStr);
            TraceLevel lvl = TraceLevelStringConvertor::str2enum(lvlStr);
            m_traceLevelMap[channel] = (int)lvl;
          }

          // path
          GET_POSSIBLE_MEMBER_AS(*props, String, "path", "", m_tracePath);

          // filename
          std::string fname;
          GET_MEMBER_AS(*props, String, "filename", "", fname);
          if (!fname.empty()) {
            m_tracePathFile = m_tracePath.empty() ? "." : m_tracePath;
            m_tracePathFile += "/";
            m_tracePathFile += fname;
            openFile();
          }
          else {
            THROW_EXC(std::logic_error, "empty file name");
            closeFile();
          }

          // file size limit
          int maxSize = 0;
          GET_POSSIBLE_MEMBER_AS(*props, Int, "maxSizeMB", "", maxSize);
          if (maxSize > 0) {
            long long ms = maxSize * FILE_SIZE_1MB;
            if (maxSize > FILE_SIZE_1MB)
              m_maxFileSize = maxSize;
          }

        }
        catch (std::exception& e) {
          std::cout << e.what() << std::endl;
        }
      }
    }

    void attachInterface(ITraceFormatService* iface)
    {
      std::lock_guard<std::mutex> guard(m_mtx);
      m_formatService = iface;
    }

    void detachInterface(ITraceFormatService* iface)
    {
      std::lock_guard<std::mutex> guard(m_mtx);
      if (m_formatService == iface)
        m_formatService = nullptr;
    }


    std::string getLogFileName()
    {
      if (m_tracePathFile.empty())
      {
        std::string logStr(m_tracePath);

        if (!logStr.empty() && *(logStr.rbegin()) != '/')
          logStr.push_back('/');

#ifdef SHAPE_PLATFORM_WINDOWS
        _mkdir(logStr.c_str());
#else
        mkdir(logStr.c_str(), 0755);
#endif

        //logStr += me->getApplicationName() + "-" + std::to_string(me->getPid()) + ".log";
        logStr += "aaa-123.log";

        return logStr;
      }
      else
      {
        return m_tracePathFile;
      }
    }

    void openFile()
    {
      static unsigned count = 0;
      if (!m_traceFile.is_open())
      {
        m_traceFile.open(getLogFileName(), std::ofstream::out | std::ofstream::trunc);
      }
    }

    void closeFile()
    {
      m_traceFile.flush();
      m_traceFile.close();
    }

    void resetFile()
    {
      closeFile();
      openFile();
    }


  };

  //-------------------------------------
  TraceFileService::TraceFileService()
  {
    imp = shape_new Imp();
  }

  TraceFileService::~TraceFileService()
  {
    delete imp;
  }

  bool TraceFileService::isValid(int level, int chan) const
  {
    return imp->isValid(level, chan);
  }

  void TraceFileService::writeMsg(int level, int channel, const char* moduleName,
    const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg)
  {
    imp->writeMsg(level, channel, moduleName, sourceFile, sourceLine, funcName, msg);
  }

  void TraceFileService::activate(const Properties *props)
  {
    imp->modify(props);
  }

  void TraceFileService::deactivate()
  {
  }

  void TraceFileService::modify(const Properties *props)
  {
    imp->modify(props);
  }

  void TraceFileService::attachInterface(ITraceFormatService* iface)
  {
    imp->attachInterface(iface);
  }

  void TraceFileService::detachInterface(ITraceFormatService* iface)
  {
    imp->detachInterface(iface);
  }

}