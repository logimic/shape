#define ITemplateService_EXPORTS

#include "ComponentTemplate.h"

#ifdef TRC_CHANNEL
#undefine TRC_CHANNEL
#endif
#define TRC_CHANNEL 1

#include "ITemplateService.h"
#include "Trace.h"

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

  void ComponentTemplate::modify(const shape::Properties *props)
  {
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
