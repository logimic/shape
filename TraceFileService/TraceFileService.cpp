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
#include "TimeString.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <map>

#ifdef SHAPE_PLATFORM_WINDOWS
#include <windows.h>
#include <direct.h>
#else
#include <dirent.h>
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
    // timestamp files
    bool m_timestamp = false;
    // max age of fles in minutes
    int m_maxAgeMinutes = 0;
    // max number of files
    int m_maxNumber = 0;

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
            //long long ms = maxSize * FILE_SIZE_1MB;
            if (maxSize > FILE_SIZE_1MB)
              m_maxFileSize = maxSize;
          }

          GET_POSSIBLE_MEMBER_AS(*props, Bool, "timestampFiles", "", m_timestamp);
          GET_POSSIBLE_MEMBER_AS(*props, Int, "maxAgeMinutes", "", m_maxAgeMinutes);
          GET_POSSIBLE_MEMBER_AS(*props, Int, "maxNumber", "", m_maxNumber);

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
      if (!m_file.is_open() && !m_fileError)
      {
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
          // remove old files or keep allowed number
          processStaleFiles(m_tracePathFile);

          std::ostringstream os;
          os << m_tracePathFile << encodeTimestamp(std::chrono::system_clock::now(), true) << '-' << m_fname;
          m_tracePathFile = os.str();
        }

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

    void selectStaleFile(std::multimap< std::chrono::system_clock::time_point, std::string> & timeFileMap
      , const std::string & fullFname
      , const std::string & fname
    ) const
    {
      size_t found = fname.find(m_fname);
      if (std::string::npos != found) {
        try {
          auto ts = parseTimestamp(fname.substr(0, found), true);
          timeFileMap.insert(std::make_pair(ts, fullFname));
        }
        catch (...) {
          //cannot convert to time => unknown time format => don't care the file
        }
      }
    }

    // remove all files older then maxAgeMinutes or don't care if maxAgeMinutes <= 0
    // keep only maxNumber of files or don't care if maxNumber <= 0
    void removeStaleFile(std::multimap< std::chrono::system_clock::time_point, std::string> & timeFileMap) const
    {
      // keep just required num of files
      if (m_maxNumber > 0) {
        size_t sz = timeFileMap.size();
        if (sz > (size_t)m_maxNumber) {
          for (size_t i = sz - m_maxNumber; i > 0; i--) {
            auto it = timeFileMap.begin();
            std::remove(it->second.c_str());
            timeFileMap.erase(it);
          }
        }
      }

      // remove older files
      if (m_maxAgeMinutes > 0) {
        std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
        tp -= std::chrono::minutes(m_maxAgeMinutes);

        for (auto & it : timeFileMap) {
          if (it.first < tp) {
            std::remove(it.second.c_str());
          }
        }
      }
    }

#ifdef SHAPE_PLATFORM_WINDOWS
    void processStaleFiles(const std::string& dir) const
    {
      using namespace std;

      WIN32_FIND_DATA fid;
      HANDLE found = INVALID_HANDLE_VALUE;

      std::multimap< std::chrono::system_clock::time_point, std::string> timeFileMap;

      string sdirect(dir);
      sdirect.append("/*");
      sdirect.append(m_fname);

      found = FindFirstFile(sdirect.c_str(), &fid);

      if (INVALID_HANDLE_VALUE == found) {
        THROW_EXC_TRC_WAR(std::logic_error, "Directory does not exist: " << PAR(dir));
      }

      do {
        if (fid.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
          continue; //skip a directory
        string fil(dir);
        fil.append(fid.cFileName);

        selectStaleFile(timeFileMap, fil, fid.cFileName);

      } while (FindNextFile(found, &fid) != 0);

      FindClose(found);

      removeStaleFile(timeFileMap);

    }

#else
    void processStaleFiles(const std::string& dirStr) const
    {
      using namespace std;

      std::multimap< std::chrono::system_clock::time_point, std::string> timeFileMap;

      DIR *dir;
      class dirent *ent;
      class stat st;

      dir = opendir(dirStr.c_str());
      if (dir == nullptr) {
        THROW_EXC_TRC_WAR(std::logic_error, "Directory does not exist: " << PAR(dirStr));
      }
      //TODO exeption if dir doesn't exists
      while ((ent = readdir(dir)) != NULL) {
        const string file_name = ent->d_name;
        const string full_file_name(dirStr + "/" + file_name);

        if (file_name[0] == '.')
          continue;

        if (stat(full_file_name.c_str(), &st) == -1)
          continue;

        const bool is_directory = (st.st_mode & S_IFDIR) != 0;

        if (is_directory)
          continue;

        selectStaleFile(timeFileMap, full_file_name, file_name);

      }
      closedir(dir);

      removeStaleFile(timeFileMap);
    }

#endif
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
