#pragma once

#include "ITraceService.h"
#include <string>
#include <vector>

namespace shape {
  
  template<typename Enum, class ConvertTable>
  class EnumStringConvertor
  {
  public:
    static Enum str2enum(const std::string & str)
    {
      for (const auto & it : ConvertTable::table()) {
        if (it.second == str) {
          return it.first;
        }
      }
      return ConvertTable::defaultEnum();
    }

    static bool str2enum(const std::string & str, Enum & enm)
    {
      for (const auto & it : ConvertTable::table()) {
        if (it.second == str) {
          enm = it.first;
          return true;
        }
      }
      enm = ConvertTable::defaultEnum();
      return false;
    }

    static const std::string & enum2str(Enum enm)
    {
      for (const auto & it : ConvertTable::table()) {
        if (it.first == enm) {
          return it.second;
        }
      }
      return ConvertTable::defaultStr();
    }

    static bool enum2str(Enum enm, std::string & str)
    {
      for (const auto & it : ConvertTable::table()) {
        if (it.first == enm) {
          str = it.second;
          return true;
        }
      }
      str = ConvertTable::defaultStr();
      return false;
    }

    static Enum int2enm(int val)
    {
      for (const auto & it : ConvertTable::table()) {
        if ((int)it.first == val) {
          return it.first;
        }
      }
      return ConvertTable::defaultEnum();
    }

    static bool int2enm(int val, Enum & enm)
    {
      for (const auto & it : ConvertTable::table()) {
        if ((int)it.first == val) {
          enm = it.first;
          return true;
        }
      }
      enm = ConvertTable::defaultEnum();
      return false;
    }

    static const std::string& int2str(int val)
    {
      for (const auto & it : ConvertTable::table()) {
        if ((int)it.first == val) {
          return it.second;
        }
      }
      return ConvertTable::defaultStr();
    }

    static bool int2str(int val, std::string& str)
    {
      for (const auto & it : ConvertTable::table()) {
        if ((int)it.first == val) {
          str = it.second;
          return true;
        }
      }
      str = ConvertTable::defaultStr();
      return false;
    }

  };

}
