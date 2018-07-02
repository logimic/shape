/**
 * Copyright 2018 Logimic,s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define ITraceService_EXPORTS
#include "TraceFileService.h"
#include "TraceLevel.h"
#include "ShapePropertiesMacros.h"
#include "ShapeDefines.h"
#include <fstream>
#include <iostream>
#include <iomanip>
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
    std::ofstream m_file;
    std::string m_path;
    std::string m_fname;
    std::string m_tracePathFile;
    long long m_maxFileSize = FILE_SIZE_1MB;
    std::mutex m_mtx;
    std::map<int, int> m_traceLevelMap;
    ITraceFormatService* m_formatService = nullptr;
    bool m_fileError = false;
    bool m_timestamp = false;

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
      if (m_fileError)
        return m_fileError;
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
        if (m_file.tellp() > m_maxFileSize)
        {
          resetFile();
        }
        if (m_formatService) {
          m_file << m_formatService->format(level, channel, moduleName, sourceFile, sourceLine, funcName, msg);
          m_file.flush();
        }
        else {
          //TODO better default format
          m_file << (int)level << ':' << channel << " " << moduleName << msg;
          m_file.flush();
        }
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
          GET_POSSIBLE_MEMBER_AS(*props, String, "path", "", m_path);

          // file size limit
          int maxSize = 0;
          GET_POSSIBLE_MEMBER_AS(*props, Int, "maxSizeMB", "", maxSize);
          if (maxSize > 0) {
            long long ms = maxSize * FILE_SIZE_1MB;
            if (maxSize > FILE_SIZE_1MB)
              m_maxFileSize = maxSize;
          }

          GET_POSSIBLE_MEMBER_AS(*props, Bool, "timestampFiles", "", m_timestamp);

          // filename
          GET_MEMBER_AS(*props, String, "filename", "", m_fname);
          if (!m_fname.empty()) {
            openFile();
          }
          else {
            THROW_EXC(std::logic_error, "empty file name");
            closeFile();
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

    void openFile()
    {
      static unsigned count = 0;

      if (m_path.empty()) {
        m_tracePathFile = "./";
      }
      else {
        m_tracePathFile = m_path;
        if (*(m_tracePathFile.rbegin()) != '/')
          m_tracePathFile.push_back('/');
#ifdef SHAPE_PLATFORM_WINDOWS
        _mkdir(m_tracePathFile.c_str());
#else
        mkdir(m_tracePathFile.c_str(), 0755);
#endif
      }
      if (!m_timestamp) {
        m_tracePathFile += m_fname;
      }
      else {
        auto timePoint = std::chrono::system_clock::now();
        auto fromMs = std::chrono::duration_cast<std::chrono::microseconds>(timePoint.time_since_epoch()).count() % 1000;
        auto time = std::chrono::system_clock::to_time_t(timePoint);
        auto tm = *std::localtime(&time);

        char buf[80];
        strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-", &tm);
        std::ostringstream os;
        os << m_tracePathFile << buf << std::setw(3) << std::setfill('0') << fromMs << '-' << m_fname;
        m_tracePathFile = os.str();
      }

      if (!m_file.is_open() && !m_fileError)
      {
       m_file.open(m_tracePathFile, std::ofstream::out | std::ofstream::trunc);
        if (!m_file.is_open()) {
          m_fileError = true;
          std::cerr << "Cannot open: " PAR(m_tracePathFile) << std::endl;
        }
      }
    }

    void closeFile()
    {
      if (m_file.is_open()) {
        m_file.flush();
        m_file.close();
      }
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
