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
