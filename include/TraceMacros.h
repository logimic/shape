#pragma once

#include "StaticTracer.h"
#include "TraceLevel.h"

#include <sstream>
#include <iomanip>

/// init default channel
#ifndef TRC_CHANNEL
#define TRC_CHANNEL 0
#endif

/// init default module name
#ifndef TRC_MNAME
#define TRC_MNAME ""
#endif

/// convenient trace macros
#ifdef _DEBUG
#define TRC_ERROR(msg)          TRC_MSG(shape::TraceLevel::Error, TRC_CHANNEL, TRC_MNAME, msg)
#define TRC_WARNING(msg)        TRC_MSG(shape::TraceLevel::Warning, TRC_CHANNEL, TRC_MNAME, msg)
#define TRC_INFORMATION(msg)    TRC_MSG(shape::TraceLevel::Information, TRC_CHANNEL, TRC_MNAME, msg)
#define TRC_DEBUG(msg)          TRC_MSG(shape::TraceLevel::Debug, TRC_CHANNEL, TRC_MNAME, msg)
#define TRC_FUNCTION_ENTER(msg) TRC_MSG(shape::TraceLevel::Debug, TRC_CHANNEL, TRC_MNAME, "[ENTER] " << msg)
#define TRC_FUNCTION_LEAVE(msg) TRC_MSG(shape::TraceLevel::Debug, TRC_CHANNEL, TRC_MNAME, "[LEAVE] " << msg)

#define TRC_ERROR_CHN(channel, mname, msg)          TRC_MSG(shape::TraceLevel::Error, channel, mname, msg)
#define TRC_WARNING_CHN(channel, mname, msg)        TRC_MSG(shape::TraceLevel::Warning, channel, mname, msg)
#define TRC_INFORMATION_CHN(channel, mname, msg)    TRC_MSG(shape::TraceLevel::Information, channel, mname, msg)
#define TRC_DEBUG_CHN(channel, mname, msg)          TRC_MSG(shape::TraceLevel::Debug, channel, mname, msg)
#define TRC_FUNCTION_ENTER_CHN(channel, mname, msg) TRC_MSG(shape::TraceLevel::Debug, channel, mname, "[ENTER] " << msg)
#define TRC_FUNCTION_LEAVE_CHN(channel, mname, msg) TRC_MSG(shape::TraceLevel::Debug, channel, mname, "[LEAVE] " << msg)

#else
#define TRC_ERROR(msg)          TRC_MSG(shape::TraceLevel::Error, TRC_CHANNEL, TRC_MNAME, msg)
#define TRC_WARNING(msg)        TRC_MSG(shape::TraceLevel::Warning, TRC_CHANNEL, TRC_MNAME, msg)
#define TRC_INFORMATION(msg)    TRC_MSG(shape::TraceLevel::Information, TRC_CHANNEL, TRC_MNAME, msg)
#define TRC_DEBUG(msg)
#define TRC_FUNCTION_ENTER(msg)
#define TRC_FUNCTION_LEAVE(msg)

#define TRC_ERROR_CHN(channel, mname, msg)          TRC_MSG(shape::TraceLevel::Error, channel, mname, msg)
#define TRC_WARNING_CHN(channel, mname, msg)        TRC_MSG(shape::TraceLevel::Warning, channel, mname, msg)
#define TRC_INFORMATION_CHN(channel, mname, msg)    TRC_MSG(shape::TraceLevel::Information, channel, mname, msg)
#define TRC_DEBUG_CHN(channel, mname, msg)
#define TRC_FUNCTION_ENTER_CHN(channel, mname, msg)
#define TRC_FUNCTION_LEAVE_CHN(channel, mname, msg)

#endif

/// exception tracing
#define THROW_EXC_TRC_WAR(extype, exmsg) { \
  TRC_WARNING("Throwing " << #extype << ": " << exmsg); \
  std::ostringstream _ostrex; \
  _ostrex << exmsg; \
  extype _ex(_ostrex.str().c_str()); \
  throw _ex; \
}

#define THROW_EXC(extype, exmsg) { \
  std::ostringstream _ostrex; \
  _ostrex << exmsg; \
  extype _ex(_ostrex.str().c_str()); \
  throw _ex; \
}

#define CATCH_EXC_TRC_WAR(extype, ex, msg) { \
  TRC_WARNING("Caught " << #extype << ": " << ex.what() << std::endl << msg); \
}

/// formats
#define PAR(par)                #par "=\"" << par << "\" "
#define PAR_HEX(par)            #par "=\"0x" << std::hex << par << std::dec << "\" "
#define NAME_PAR(name, par)     #name "=\"" << par << "\" "
#define NAME_PAR_HEX(name,par)  #name "=\"0x" << std::hex << par << std::dec << "\" "

/// auxiliar macro
#define TRC_MSG(level, channel, mname, msg) \
if (shape::Tracer::get().isValid((int)level, channel)) { \
  std::ostringstream _ostrmsg; \
  _ostrmsg << msg << std::endl; \
  shape::Tracer::get().writeMsg((int)level, channel, mname, __FILE__, __LINE__, __FUNCTION__, _ostrmsg.str()); \
}
