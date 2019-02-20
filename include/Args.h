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

#include "ShapeDefines.h"
#include <vector>
#include <sstream>
#include <string.h>
#include <iterator>

namespace shape {
  class Args
  {
  public:
    Args() = delete;
    
    Args(int argc, char** argv)
    {
      for (int ia = 0; ia < argc; ia++) {
        m_argsVect.push_back(argv[ia]);
      }
      initArgs();
    }

    Args(const std::string args)
    {
      std::stringstream sstr(args);

      std::istream_iterator<std::string> it(sstr);
      std::istream_iterator<std::string> end;
      m_argsVect = std::vector<std::string>(it, end);

      initArgs();
    }
    
    Args(const std::vector<std::string>& args)
    {
      m_argsVect = args;
      initArgs();
    }
    
    ~Args()
    {
      if (m_argc > 0)
      {
        for (int n = 0; n < m_argc; n++)
          delete[] m_argv[n];
        delete[] m_argv;
      }
    }

    int* argc() { return &m_argc; }
    char ** argv() { return m_argv; }
    const std::vector<std::string>& gestArgs() { return m_argsVect; }

  private:
    void initArgs()
    {
      m_argc = static_cast<int>(m_argsVect.size());
      if (m_argc > 0)
      {
        m_argv = shape_new char *[m_argc + 1];
        int n = 0;
        for (; n < m_argc; n++)
        {
          m_argv[n] = shape_new char[m_argsVect[n].size() + 1];
          strcpy(m_argv[n], m_argsVect[n].c_str());
        }
        m_argv[n] = nullptr; //must be ended with null
      }
    }
    
    int m_argc = 0;
    char** m_argv = nullptr;
    std::vector<std::string> m_argsVect;
  };
}
