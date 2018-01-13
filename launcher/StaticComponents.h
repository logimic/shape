#pragma once

//TODO generate from CMakeLists.txt

#include <ShapeComponent.h>
#include <StaticComponentMap.h>
#include "Trace.h"

#ifdef SHAPE_STATIC_LIBS

extern "C" {
  const shape::ComponentMeta& get_component_shape__TraceFormatService(unsigned long* compiler, unsigned long* typehash);
  const shape::ComponentMeta& get_component_shape__TraceFileService(unsigned long* compiler, unsigned long* typehash);
  const shape::ComponentMeta& get_component_shape__CommandService(unsigned long* compiler, unsigned long* typehash);
  const shape::ComponentMeta& get_component_shape__LauncherService(unsigned long* compiler, unsigned long* typehash);
}

void staticInit()
{
  void* component = nullptr;
  unsigned long compiler = 0;
  unsigned long hashcode = 0;
  unsigned long expectedCompiler = (unsigned long)SHAPE_PREDEF_COMPILER;
  unsigned long expectedHashcode = std::type_index(typeid(shape::ComponentMeta)).hash_code();

  // get components
  shape::ComponentMap::get().setComponent(&get_component_shape__TraceFormatService(&compiler, &hashcode));
  shape::ComponentMap::get().setComponent(&get_component_shape__TraceFileService(&compiler, &hashcode));
  shape::ComponentMap::get().setComponent(&get_component_shape__CommandService(&compiler, &hashcode));
  shape::ComponentMap::get().setComponent(&get_component_shape__LauncherService(&compiler, &hashcode));
}

#else
extern "C" {
  const shape::ComponentMeta& get_component_shape__LauncherService(unsigned long* compiler, unsigned long* typehash);
}

void staticInit()
{
  void* component = nullptr;
  unsigned long compiler = 0;
  unsigned long hashcode = 0;
  unsigned long expectedCompiler = (unsigned long)SHAPE_PREDEF_COMPILER;
  unsigned long expectedHashcode = std::type_index(typeid(shape::ComponentMeta)).hash_code();

  shape::ComponentMap::get().setComponent(&get_component_shape__LauncherService(&compiler, &hashcode));
}
#endif
