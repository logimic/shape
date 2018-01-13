#pragma once

#include "EnumStringConvertor.h"

namespace shape {
  
  enum class TraceLevel
  {
    Error,
    Warning,
    Information,
    Debug
  };

  class TraceLevelConvertTable
  {
  public:
    static const std::vector<std::pair<TraceLevel, std::string>>& table()
    {
      static std::vector <std::pair<TraceLevel, std::string>> table = {
        { TraceLevel::Error, "ERR" },
        { TraceLevel::Warning, "WAR" },
        { TraceLevel::Information, "INF" },
        { TraceLevel::Debug, "DBG" }
      };
      return table;
    }
    static TraceLevel defaultEnum()
    {
      return TraceLevel::Information;
    }
    static const std::string& defaultStr()
    {
      static std::string u("unknown");
      return u;
    }
  };

  typedef EnumStringConvertor<TraceLevel, TraceLevelConvertTable> TraceLevelStringConvertor;

}
