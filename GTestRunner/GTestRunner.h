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

#include "ShapeProperties.h"
#include <string>
#include <thread>
#include <set>

namespace shape {
  class ILaunchService;
  class ITraceService;

  class GTestRunner
  {
  public:
    GTestRunner();
    virtual ~GTestRunner();

    void activate(const Properties *props = 0);
    void deactivate();
    void modify(const Properties *props);

    void attachInterface(ILaunchService* iface);
    void detachInterface(ILaunchService* iface);

    void attachInterface(ITraceService* iface);
    void detachInterface(ITraceService* iface);

  private:
    class Imp;
    Imp *m_imp = nullptr;
  };
}
