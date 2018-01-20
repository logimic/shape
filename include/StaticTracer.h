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

#pragma once

#include "TraceMacros.h"
#include "ITraceService.h"

#include <map>
#include <vector>
#include <mutex>
#include <sstream>
#include <iomanip>

/// init static tracer
#define TRC_INIT_MNAME(moduleName) \
namespace shape { \
  shape::Tracer& shape::Tracer::get() { \
    static Tracer tracer(#moduleName); \
    tracer.setBuffered(); \
    return tracer; \
  } \
  static Tracer& _shape_log(Tracer::get());\
}

#ifndef SHAPE_STATIC_LIBS
#define TRC_INIT_MODULE(moduleName) TRC_INIT_MNAME(moduleName)
#else
#define TRC_INIT_MODULE(moduleName)
#endif

namespace shape {
  class Tracer {
  public:
    Tracer() = delete;
    Tracer & operator = (const Tracer& t) = delete;
    Tracer(const Tracer& t) = delete;

    bool isValid(int level, int chan)
    {
      std::lock_guard<std::recursive_mutex> guard(m_trcMtx);

      if (m_trcMap.empty()) {
        return m_bufferedFlag;
      }

      for (const auto & ser : m_trcMap) {
        if (ser.first->isValid(level, chan))
          return true;
      }

      return false;
    };

    void writeMsg(int level, int channel, const char* moduleName,
      const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg)
    {
      std::lock_guard<std::recursive_mutex> guard(m_trcMtx);

      if (m_trcMap.empty() && m_bufferedFlag) {
        m_buffer.push_back(BufferedMessage(level, channel, moduleName, sourceFile, sourceLine, funcName, msg));
      }

      for (const auto & ser : m_trcMap) {
        if (ser.first->isValid(level, channel))
          ser.first->writeMsg(level, channel, moduleName, sourceFile, sourceLine, funcName, msg);
      }
    }

    const std::string& getModuleName() const
    {
      return m_moduleName;
    };

    static Tracer& get();

    void addTracerService(ITraceService* ls)
    {
      std::lock_guard<std::recursive_mutex> guard(m_trcMtx);

      auto found = m_trcMap.find(ls);
      if (m_trcMap.end() != found) {
        found->second++;
      }
      else {
        m_trcMap.insert(std::make_pair(ls, 1));
      }
    };

    void removeTracerService(ITraceService* ls)
    {
      std::lock_guard<std::recursive_mutex> guard(m_trcMtx);

      auto found = m_trcMap.find(ls);
      if (m_trcMap.end() != found) {
        if (--(found->second) <= 0)
          m_trcMap.erase(found);
      }
    }

    void setBuffered() { m_bufferedFlag = true; }
    void resetBuffered() { m_bufferedFlag = false; }
    void writeBuffer()
    {
      std::lock_guard<std::recursive_mutex> guard(m_trcMtx);

      for (const auto & item : m_buffer) {
        int level = item.m_level;
        int channel = item.m_channel;
        for (const auto & ser : m_trcMap) {
          if (ser.first->isValid(level, channel))
            ser.first->writeMsg(level, channel, item.m_moduleName, item.m_sourceFile, item.m_sourceLine, item.m_funcName, item.m_msg);
        }
      }
      m_buffer.clear();
    }

  private:
    Tracer(const std::string& moduleName)
      : m_moduleName(moduleName)
    {
    }

    std::map<ITraceService*, int> m_trcMap;
    std::string m_moduleName;

    std::recursive_mutex m_trcMtx;

    //for preliminary tracing
    struct BufferedMessage {
      int m_level;
      int m_channel;
      const char* m_moduleName;
      const char* m_sourceFile;
      int m_sourceLine;
      const char* m_funcName;
      std::string m_msg;
      
      BufferedMessage(int level, int channel, const char* moduleName,
        const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg)
        :m_level(level)
        , m_channel(channel)
        , m_moduleName(moduleName)
        , m_sourceFile(sourceFile)
        , m_sourceLine(sourceLine)
        , m_funcName(funcName)
        , m_msg(msg)
      {}
    };

    //TODO to file
    std::vector<BufferedMessage> m_buffer;

    //TODO buffered flag to be set/reset outside
    bool m_bufferedFlag = false;

  };
}
