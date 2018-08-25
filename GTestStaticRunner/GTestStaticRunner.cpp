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

#include "GTestStaticRunner.h"
#include "ILaunchService.h"
#include "Trace.h"
#include <chrono>
#include <iostream>
#include <set>

#include "gtest/gtest.h"

namespace shape {
  GTestStaticRunner::GTestStaticRunner()
  {
  }

  GTestStaticRunner::~GTestStaticRunner()
  {
    TRC_FUNCTION_ENTER("");
    TRC_INFORMATION(std::endl <<
      "******************************" << std::endl <<
      "GTestStaticRunner instance deactivate" << std::endl <<
      "******************************"
    );

    //graceful thread finish
    m_runTreadFlag = false;
    if (m_thread.joinable()) {
      m_thread.join();
    }

    TRC_FUNCTION_LEAVE("")
  }

  void GTestStaticRunner::runAllTests(ILaunchService* iface)
  {
    TRC_FUNCTION_ENTER("");
    TRC_INFORMATION(std::endl <<
      "******************************" << std::endl <<
      "GTestStaticRunner instance activate" << std::endl <<
      "******************************"
    );

    m_iLaunchService = iface;
    m_thread = std::thread([this]() { this->runThread(); });

    TRC_FUNCTION_LEAVE("")

  }
  
  class Args
  {
  public:
    Args() = delete;
    Args(const std::vector<std::string>& args)
    {
      m_argc = args.size();
      if (m_argc > 0)
      {
        m_argv = shape_new char *[m_argc + 1];
        int n = 0;
        for (; n < m_argc; n++)
        {
          m_argv[n] = shape_new char[args[n].size() + 1];
          strcpy(m_argv[n], args[n].c_str());
        }
        m_argv[n] = nullptr; //must be ended with null
      }
    }
    ~Args()
    {
      if (m_argc > 0)
      {
        int n = 0;
        for (int n = 0; n < m_argc; n++)
          delete[] m_argv[n];
        delete[] m_argv;
      }
    }

    int* argc() { return &m_argc; }
    char ** argv() { return m_argv; }

  private:
    int m_argc = 0;
    char** m_argv = nullptr;
  };

  void GTestStaticRunner::runThread()
  {
    TRC_FUNCTION_ENTER("");

    Args args(m_iLaunchService->getCommandLine());
    ::testing::InitGoogleTest(args.argc(), args.argv());
    
    int retval = RUN_ALL_TESTS();

    m_iLaunchService->exit(retval);

    TRC_FUNCTION_LEAVE("")
  }

}
