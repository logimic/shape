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

#include "Example1_Thread.h"

#include "Trace.h"
#include <chrono>
#include <iostream>

#include "shape__Example1_Thread.hxx"

TRC_INIT_MODULE(shape::Example1_Thread);

namespace shape {
  Example1_Thread::Example1_Thread()
  {
    TRC_FUNCTION_ENTER("");
    TRC_FUNCTION_LEAVE("")
  }

  Example1_Thread::~Example1_Thread()
  {
    TRC_FUNCTION_ENTER("");
    TRC_FUNCTION_LEAVE("")
  }

  void Example1_Thread::activate(const Properties *props)
  {
    (void)props; //silence -Wunused-parameter
    TRC_FUNCTION_ENTER("");
    TRC_INFORMATION(std::endl <<
      "******************************" << std::endl <<
      "Example1_Thread instance activate" << std::endl <<
      "******************************"
    );

    //invoking thread by lambda
    m_thread = std::thread([this]() { this->runTread(); });

    TRC_FUNCTION_LEAVE("")
  }

  void Example1_Thread::deactivate()
  {
    TRC_FUNCTION_ENTER("");
    TRC_INFORMATION(std::endl <<
      "******************************" << std::endl <<
      "Example1_Thread instance deactivate" << std::endl <<
      "******************************"
    );

    //graceful thread finish
    m_runTreadFlag = false;
    if (m_thread.joinable()) {
      m_thread.join();
    }

    TRC_FUNCTION_LEAVE("")
  }

  void Example1_Thread::modify(const Properties *props)
  {
    (void)props; //silence -Wunused-parameter
  }

  void Example1_Thread::attachInterface(ITraceService* iface)
  {
    shape::Tracer::get().addTracerService(iface);
  }

  void Example1_Thread::detachInterface(ITraceService* iface)
  {
    shape::Tracer::get().removeTracerService(iface);
  }

  ////////////////////////
  void Example1_Thread::runTread()
  {
    TRC_FUNCTION_ENTER("");

    static int num = 0;
    
    while (m_runTreadFlag) {
      std::cout << std::endl << num++;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    TRC_FUNCTION_LEAVE("")
  }

}
