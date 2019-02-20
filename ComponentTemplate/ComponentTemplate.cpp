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

#define ITemplateService_EXPORTS

#include "ComponentTemplate.h"

#ifdef TRC_CHANNEL
#undef TRC_CHANNEL
#endif
#define TRC_CHANNEL 1

#include "ITemplateService.h"
#include "Trace.h"
#include <algorithm>

#include "shapeExpl__ComponentTemplate.hxx"

TRC_INIT_MODULE(shapeExpl::ComponentTemplate);

namespace shapeExpl {
  ComponentTemplate::ComponentTemplate()
  {
    TRC_FUNCTION_ENTER("");
    TRC_FUNCTION_LEAVE("")
  }

  ComponentTemplate::~ComponentTemplate()
  {
    TRC_FUNCTION_ENTER("");
    TRC_FUNCTION_LEAVE("")
  }

  std::string ComponentTemplate::doService(const std::string & str) const
  {
    TRC_FUNCTION_ENTER(PAR(str));
    std::string revStr(str);
    std::reverse(revStr.begin(), revStr.end());
    TRC_FUNCTION_LEAVE(PAR(revStr));
    return revStr;
  }

  void ComponentTemplate::activate(const shape::Properties *props)
  {
    TRC_FUNCTION_ENTER(PAR(props));
    TRC_INFORMATION(std::endl <<
      "******************************" << std::endl <<
      "ComponentTemplate instance activate" << std::endl <<
      "******************************"
    );
    TRC_FUNCTION_LEAVE("")
  }

  void ComponentTemplate::deactivate()
  {
    TRC_FUNCTION_ENTER("");
    TRC_INFORMATION(std::endl <<
      "******************************" << std::endl <<
      "ComponentTemplate instance deactivate" << std::endl <<
      "******************************"
    );
    TRC_FUNCTION_LEAVE("")
  }

  void ComponentTemplate::modify(const shape::Properties *props)
  {
    (void)props; //silence -Wunused-parameter
  }

  void ComponentTemplate::attachInterface(shape::ITraceService* iface)
  {
    shape::Tracer::get().addTracerService(iface);
  }

  void ComponentTemplate::detachInterface(shape::ITraceService* iface)
  {
    shape::Tracer::get().removeTracerService(iface);
  }

}
