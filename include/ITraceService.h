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

#include <string>
#include <vector>

#include "ShapeDefines.h"

#ifdef ITraceService_EXPORTS
#define ITRACE_SERVICE_DECLSPEC SHAPE_ABI_EXPORT
#else
#define ITRACE_SERVICE_DECLSPEC SHAPE_ABI_IMPORT
#endif

namespace shape {
  class ITRACE_SERVICE_DECLSPEC ITraceService
  {
  public:
    virtual bool isValid(int level, int channel) const = 0;
    virtual void writeMsg(int level, int channel, const char* moduleName,
      const char* sourceFile, int sourceLine, const char* funcName, const std::string & msg) = 0;
    virtual ~ITraceService() {}
  };
}
