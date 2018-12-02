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

#include "LibLoader.h"
#include "Trace.h"
#include <algorithm>
#include <fstream>

#ifdef SHAPE_PLATFORM_POSIX
#include <dlfcn.h>
#else
#include <Windows.h>
#endif

using namespace std;

#ifdef SHAPE_PLATFORM_WINDOWS
const std::string SHL_SUFFIX(".dll");
const std::string SHL_PREFIX;
#else
const std::string SHL_SUFFIX(".so");
const std::string SHL_PREFIX("lib");
#endif

namespace shape {

  LibLoader::LibLoader()
  {
  }

  LibLoader::~LibLoader()
  {
  }

  bool LibLoader::isOpenedLibrary()
  {
    return m_lib != nullptr;
  }

  void formatPath(std::string& pth)
  {
    if (!pth.empty()) {
      std::replace(pth.begin(), pth.end(), '\\', '/');
      if (pth.back() != '/')
        pth.append("/");
    }
  }

  bool LibLoader::findLibrary(const std::string& deploymentPath, const std::string& libraryPath, const std::string& libraryName)
  {
    TRC_FUNCTION_ENTER(PAR(deploymentPath) << PAR(libraryPath) << PAR(libraryName));

    // decorate library name by system specific prefix & suffix
    ostringstream o;
    o << SHL_PREFIX << libraryName << SHL_SUFFIX;
    m_libraryName = o.str();

    std::string dpath = deploymentPath;
    formatPath(dpath);
    std::string lpath = libraryPath;
    formatPath(lpath);

    bool exists = false;
    
    ostringstream olpf1;
    olpf1 << dpath << lpath;
    m_libraryPath = olpf1.str();
    olpf1 << m_libraryName;
    m_libraryPathFile = olpf1.str();
    m_triedPaths.push_back(m_libraryPathFile);
    std::ifstream f1(m_libraryPathFile);
    exists = f1.good();

    if (!exists) {
      if (!lpath.empty()) {
        // try libraryPath/libraryName (legacy)
        m_libraryPath = lpath;
      }
      else {
        // try deploymentPath/libraryname (legacy)
        m_libraryPath = dpath;
      }
      ostringstream olpf2;
      olpf2 << m_libraryPath << m_libraryName;
      m_libraryPathFile = olpf2.str();
      m_triedPaths.push_back(m_libraryPathFile);
      std::ifstream f2(m_libraryPathFile);
      exists = f2.good();
    }

    TRC_FUNCTION_LEAVE(PAR(exists));
    return exists;
  }



#ifdef SHAPE_PLATFORM_WINDOWS

  void LibLoader::openLibrary()
  {
    TRC_FUNCTION_ENTER(PAR(m_libraryPath) << PAR(m_libraryName));
    
    if (nullptr == m_lib) {

      //set explicit search path to load dependecies dll 
      const unsigned BUFSIZE = 4096;
      DWORD  retval = 0;
      BOOL   success;
      CHAR  buffer[BUFSIZE] = "";
      LPSTR* lppPart = { NULL };

      retval = GetFullPathName(
        m_libraryPath.c_str(),
        BUFSIZE,
        buffer,
        lppPart);

      m_libraryPath = buffer;
      SetDllDirectory(buffer);
      m_libraryPathFile = m_libraryName;

      m_lib = LoadLibrary(m_libraryPathFile.c_str());
      if (nullptr == m_lib) {
        TRC_WARNING("Error load: " << NAME_PAR(errno, GetLastError()) << PAR(m_libraryPathFile));
      }

      //reset dll search path to default
      SetDllDirectory(nullptr);
    }

    TRC_FUNCTION_LEAVE("");
  }

  void LibLoader::closeLibrary()
  {
    TRC_FUNCTION_ENTER(PAR(m_libraryPath) << PAR(m_libraryName));
    if (nullptr != m_lib) {
      if (!FreeLibrary((HMODULE)m_lib)) {
        TRC_WARNING("Error unload: " << NAME_PAR(errno, GetLastError()) << PAR(m_libraryPathFile));
      }
      m_lib = nullptr;
    }
    TRC_FUNCTION_LEAVE("");
  }

  void * LibLoader::getFunction(const std::string& functionName) const
  {
    void * function(nullptr);
    if (m_lib) {
      function = GetProcAddress((HMODULE)m_lib, functionName.c_str());
      if (nullptr == function) {
        TRC_WARNING("GetProcAddress(): " << PAR(functionName) << " failed with error: " << GetLastError());
      }
    }
    else {
      TRC_WARNING(PAR(m_lib));
    }
    return function;
  }

#else
  void LibLoader::openLibrary()
  {
    TRC_FUNCTION_ENTER(PAR(m_libraryPath) << PAR(m_libraryName));
    if (nullptr == m_lib) {
      int flags = RTLD_LAZY | RTLD_LOCAL;
      m_lib = dlopen(m_libraryPathFile.c_str(), flags);
      if (nullptr == m_lib) {
        string errnoStr(dlerror());
        THROW_EXC_TRC_WAR(std::logic_error, "Error load: " << PAR(errnoStr) << PAR(m_libraryPathFile));
    }
  }
    TRC_FUNCTION_LEAVE("");
}

  void LibLoader::closeLibrary()
  {
    TRC_FUNCTION_ENTER(PAR(m_libraryPath) << PAR(m_libraryPathFile));
    if (nullptr != m_lib) {
      if (dlclose(m_lib))
      {
        string errnoStr(dlerror());
        TRC_WARNING("Error unload: " << PAR(errnoStr) << PAR(m_libraryPathFile));
      }
      m_lib = nullptr;
    }
    TRC_FUNCTION_LEAVE("");
  }

  void * LibLoader::getFunction(const std::string& methodName) const
  {
    void * function(nullptr);
    if (m_lib) {
      function = dlsym(m_lib, methodName.c_str());
      if (nullptr == function) {
        const char* err = dlerror();
        TRC_WARNING("dlsym(): " << PAR(methodName) << " failed with error: " << (err ? err : "NULL"));
      }
    }
    else {
      TRC_WARNING(PAR(m_lib));
    }
    return function;
  }

#endif

}
