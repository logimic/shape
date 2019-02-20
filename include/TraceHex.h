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

#include <sstream>
#include <iomanip>

/// formats
#define MEM_HEX_CHAR(ptr, len)  shape::TracerMemHexChar((const void*)ptr, static_cast<long>(len), ' ')
#define MEM_HEX(ptr, len)       shape::TracerMemHex((const void*)ptr, static_cast<long>(len), '.')

namespace shape {
  class TracerMemHex
  {
  public:
    TracerMemHex() = delete;
    TracerMemHex & operator = (const TracerMemHex& t) = delete;
    TracerMemHex(const TracerMemHex& t) = delete;

    TracerMemHex(const void* buf, long len, char separator)
    {
      if (len <= 0) return;
      m_osbyte.setf(std::ios::hex, std::ios::basefield);
      m_osbyte.fill('0');
      long i = 0;
      while (true) {
        uint8_t bt = ((const uint8_t*)buf)[i++];
        m_osbyte << std::setw(2) << (short int)bt;
        if (i == len)
          break;
        m_osbyte << separator;
      }
    }

    friend std::ostream& operator<<(std::ostream& os, const TracerMemHex& t) {
      os << t.m_osbyte.str();
      return os;
    }

  private:
    std::ostringstream m_osbyte;
  };

  class TracerMemHexChar
  {
  public:
    TracerMemHexChar() = delete;
    TracerMemHexChar & operator = (const TracerMemHexChar& t) = delete;
    TracerMemHexChar(const TracerMemHexChar& t) = delete;

    TracerMemHexChar(const void* buf, size_t len, char separator) {
      if (len <= 0) return;
      m_osbyte.setf(std::ios::hex, std::ios::basefield);
      m_osbyte.fill('0');
      size_t i = 0;
      while (true) {
        uint8_t bt = ((const unsigned char*)buf)[i++];
        m_osbyte << std::setw(2) << (short int)bt << separator;
        m_oschar << (isgraph(bt) ? (char)bt : '.');

        if (i == len) {
          //align last
          while (i++ % 16) {
            m_osbyte << "   ";
            m_oschar << ' ';
          }
          m_osbyte << "  " << m_oschar.str();
          break;
        }

        if (0 == (i % 16)) {
          m_osbyte << "  " << m_oschar.str();
          m_oschar.seekp(0);
          m_osbyte << std::endl;
        }
      }
    }

    friend std::ostream& operator<<(std::ostream& os, const TracerMemHexChar& t) {
      os << t.m_osbyte.str();
      return os;
    }

  private:
    std::ostringstream m_osbyte;
    std::ostringstream m_oschar;
  };
}
