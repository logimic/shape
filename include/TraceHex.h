#pragma once

#include <sstream>
#include <iomanip>

/// formats
#define MEM_HEX_CHAR(ptr, len)  shape::TracerMemHexChar((const void*)ptr, len, ' ')
#define MEM_HEX(ptr, len)       shape::TracerMemHex((const void*)ptr, len, '.')

namespace shape {
  class TracerMemHex
  {
  public:
    TracerMemHex() = delete;
    TracerMemHex & operator = (const TracerMemHex& t) = delete;
    TracerMemHex(const TracerMemHex& t) = delete;

    TracerMemHex(const void* buf, long len, char separator)
    {
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

    TracerMemHexChar(const void* buf, const long len, char separator) {
      m_osbyte.setf(std::ios::hex, std::ios::basefield);
      m_osbyte.fill('0');
      long i = 0;
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
