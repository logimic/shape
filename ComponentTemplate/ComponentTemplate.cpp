#define ITemplateService_EXPORTS

#include "ComponentTemplate.h"

#ifdef TRC_CHANNEL
#undefine TRC_CHANNEL
#endif
#define TRC_CHANNEL 1

#include "ITemplateService.h"
#include "Trace.h"

#include "shape__ComponentTemplate.hxx"

TRC_INIT_MODULE(shape::ComponentTemplate);

namespace shape {
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

  void ComponentTemplate::activate(const Properties *props)
  {
    TRC_FUNCTION_ENTER("");
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

  void ComponentTemplate::modify(const Properties *props)
  {
  }

  void ComponentTemplate::attachInterface(ITraceService* iface)
  {
    shape::Tracer::get().addTracerService(iface);
  }

  void ComponentTemplate::detachInterface(ITraceService* iface)
  {
    shape::Tracer::get().removeTracerService(iface);
  }

}
