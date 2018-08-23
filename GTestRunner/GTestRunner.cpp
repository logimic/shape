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

#include "GTestRunner.h"
#include "ILaunchService.h"
#include "Trace.h"
#include <chrono>
#include <iostream>
#include <set>

#include "gtest/gtest.h"

#include "shape__GTestRunner.hxx"

#ifdef TRC_CHANNEL
#undef TRC_CHANNEL
#endif
#define TRC_CHANNEL 0

TRC_INIT_MODULE(shape::GTestRunner);

namespace shape {

  class GTestRunner::Imp 
  {
  private:
    ILaunchService* m_iLaunchService = nullptr;
    bool m_runTreadFlag = true;
    std::thread m_thread;

  public:
    Imp()
    {
      TRC_FUNCTION_ENTER("");
      TRC_FUNCTION_LEAVE("")
    }

    ~Imp()
    {
      TRC_FUNCTION_ENTER("");
      TRC_FUNCTION_LEAVE("")
    }

    void activate(const Properties *props)
    {
      TRC_FUNCTION_ENTER("");
      TRC_INFORMATION(std::endl <<
        "******************************" << std::endl <<
        "GTestRunner instance activate" << std::endl <<
        "******************************"
      );

      m_thread = std::thread([this]() { this->runTread(); });

      TRC_FUNCTION_LEAVE("")
    }

    void deactivate()
    {
      TRC_FUNCTION_ENTER("");
      TRC_INFORMATION(std::endl <<
        "******************************" << std::endl <<
        "GTestRunner instance deactivate" << std::endl <<
        "******************************"
      );

      //graceful thread finish
      m_runTreadFlag = false;
      if (m_thread.joinable()) {
        m_thread.join();
      }

      TRC_FUNCTION_LEAVE("")
    }

    void modify(const Properties *props)
    {
    }

    void attachInterface(ILaunchService* iface)
    {
      m_iLaunchService = iface;
    }

    void detachInterface(ILaunchService* iface)
    {
      if (m_iLaunchService == iface) {
        m_iLaunchService = nullptr;
      }
    }

    ////////////////////////
  private:
    void runTread()
    {
      TRC_FUNCTION_ENTER("");

      //static int num = 0;

      //while (m_runTreadFlag) {
      //  num++;
      //  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      //}

      //::testing::InitGoogleTest(&argc, argv);
      //int argc = 1;
      //char argv[] = { "app" };

      char  arg0[] = "app";
      char* argv[] = { &arg0[0], NULL };
      int   argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

      ::testing::InitGoogleTest(&argc, (char**)&argv);
      int retval = RUN_ALL_TESTS();
      std::cout << std::endl << "RUN_ALL_TESTS" << PAR(retval) << std::endl;

      m_iLaunchService->exit(retval);

      TRC_FUNCTION_LEAVE("")
    }

  };


  ///////////////////////////////////////////
  GTestRunner::GTestRunner()
  {
    m_imp = shape_new Imp();
  }

  GTestRunner::~GTestRunner()
  {
    delete m_imp;
  }

  void GTestRunner::activate(const Properties *props)
  {
    m_imp->activate(props);
  }

  void GTestRunner::deactivate()
  {
    m_imp->deactivate();
  }

  void GTestRunner::modify(const Properties *props)
  {
    m_imp->modify(props);
  }

  void GTestRunner::attachInterface(ILaunchService* iface)
  {
    m_imp->attachInterface(iface);
  }

  void GTestRunner::detachInterface(ILaunchService* iface)
  {
    m_imp->detachInterface(iface);
  }

  void GTestRunner::attachInterface(ITraceService* iface)
  {
    shape::Tracer::get().addTracerService(iface);
  }

  void GTestRunner::detachInterface(ITraceService* iface)
  {
    shape::Tracer::get().removeTracerService(iface);
  }

	TEST(Case1, Test1) {
	bool var = true;
	EXPECT_TRUE(true == var);
	}


}
