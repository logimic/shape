#pragma once

#include "Trace.h"
#include <exception>
#include <algorithm>
#include <sstream>
#include <chrono>

namespace shape {
  /// \brief parse time up to milliseconds granularity in ISO8601 format YYY-MM-DDThh:mm:ss.mss
  /// \param [in] ts string to be parsed
  /// \param [in] mss milliseconds parse flag
  /// \return encoded time point
  std::chrono::time_point<std::chrono::system_clock> parseTimestampISO8601(const std::string& ts, bool mss = false)
  {
    std::chrono::time_point<std::chrono::system_clock> retval;
    bool succ = false;

    if (!ts.empty()) {
      int tm_year = 0, tm_mon = 1;

      time_t rawtime;
      tm * tm1;
      time(&rawtime);
      tm1 = localtime(&rawtime);

      std::string buf(ts);
      std::replace(buf.begin(), buf.end(), '-', ' ');
      std::replace(buf.begin(), buf.end(), 'T', ' ');
      std::replace(buf.begin(), buf.end(), ':', ' ');
      if (mss) {
        std::replace(buf.begin(), buf.end(), '.', ' ');
      }

      std::istringstream is(buf);
      is >> tm_year >> tm_mon >> tm1->tm_mday >> tm1->tm_hour >> tm1->tm_min >> tm1->tm_sec;
      int msec = 0;
      if (mss) {
        is >> msec;
      }
      tm1->tm_year = tm_year - 1900;
      tm1->tm_mon = tm_mon - 1;

      time_t tt = mktime(tm1);

      if (tt >= 0) {
        retval = std::chrono::system_clock::from_time_t(tt);
        if (mss) {
          retval += std::chrono::milliseconds(msec);
        }
        succ = true;
      }
    }

    if (!succ) {
      std::ostringstream os;
      os << "cannot convert ISO8601 string to time: " << ts;
      throw std::invalid_argument(os.str());
    }

    return retval;
  }

  /// \brief parse time up to  YYY-MM-DDThh:mm:ss.mss granularity in format YYY-MM-DD-hh-mm-ss-mss
  /// \param [in] ts string to be parsed
  /// \param [in] mss milliseconds parse flag
  /// \return encoded time point
  std::chrono::time_point<std::chrono::system_clock> parseTimestamp(const std::string& ts, bool mss = false)
  {
    std::chrono::time_point<std::chrono::system_clock> retval;
    bool succ = false;

    if (!ts.empty()) {
      int tm_year = 0, tm_mon = 1;

      time_t rawtime;
      tm * tm1;
      time(&rawtime);
      tm1 = localtime(&rawtime);

      std::string buf(ts);
      std::replace(buf.begin(), buf.end(), '-', ' ');

      std::istringstream is(buf);
      is >> tm_year >> tm_mon >> tm1->tm_mday >> tm1->tm_hour >> tm1->tm_min >> tm1->tm_sec;
      int msec = 0;
      if (mss) {
        is >> msec;
      }
      tm1->tm_year = tm_year - 1900;
      tm1->tm_mon = tm_mon - 1;

      time_t tt = mktime(tm1);

      if (tt >= 0) {
        retval = std::chrono::system_clock::from_time_t(tt);
        if (mss) {
          retval += std::chrono::milliseconds(msec);
        }
        succ = true;
      }
    }

    if (!succ) {
      std::ostringstream os;
      os << "cannot convert string to time: " << ts;
      throw std::invalid_argument(os.str());
    }

    return retval;
  }

  /// \brief Encode timestamp in ISO8601 format  YYY-MM-DDThh:mm:ss.mss
  /// \param [in] ts timestamp to be encoded
  /// \param [in] tzone add ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100)
  /// \param [in] mss milliseconds add flag
  /// \return encoded string
  std::string encodeTimestampISO8601(std::chrono::time_point<std::chrono::system_clock> ts, bool tzone = false, bool mss = false)
  {
    using namespace std::chrono;

    auto time = system_clock::to_time_t(ts);
    //auto tm = *std::gmtime(&time);
    auto tm = *std::localtime(&time);
    char buf[80];

    if (mss) {
      auto fromMs = duration_cast<milliseconds>(ts.time_since_epoch()).count() % 1000;

      if (tzone) {
        strftime(buf, sizeof(buf), "%FT%T.mmm%z", &tm);
      }
      else {
        strftime(buf, sizeof(buf), "%FT%T.mmm", &tm);
      }

      // convert to ISO8601 Date (Extend) format
      std::string str(buf);
      std::ostringstream os;
      os.fill('0'); os.width(3);
      os << fromMs;
      str.replace(str.find("mmm"), 3, os.str());
      str.insert(str.size() - 2, 1, ':');
      return str;
    }
    else {
      char buf[80];

      if (tzone) {
        strftime(buf, sizeof(buf), "%FT%T%z", &tm);
      }
      else {
        strftime(buf, sizeof(buf), "%FT%T", &tm);
      }

      std::string str(buf);

      return str;
    }
  }

  /// \brief Encode timestamp in format YYY-MM-DD-hh-mm-ss-mss
  /// \param [in] ts timestamp to be encoded
  /// \param [in] mss milliseconds add flag
  /// \return encoded string
  std::string encodeTimestamp(std::chrono::time_point<std::chrono::system_clock> ts, bool mss = false)
  {
    using namespace std::chrono;

    auto time = system_clock::to_time_t(ts);
    //auto tm = *std::gmtime(&time);
    auto tm = *std::localtime(&time);
    char buf[80];

    if (mss) {
      auto fromMs = duration_cast<milliseconds>(ts.time_since_epoch()).count() % 1000;
      strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S-", &tm);
      std::ostringstream os;
      os << buf;
      os.fill('0'); os.width(3);
      os << fromMs;
      return os.str();
    }
    else {
      strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S", &tm);
      std::string str(buf);
      return str;
    }
  }

}
