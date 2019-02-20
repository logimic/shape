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

//TODO generate from CMakeLists.txt

#include <ShapeComponent.h>
#include <StaticComponentMap.h>
#include "Trace.h"

#ifdef SHAPE_STATIC_LIBS

extern "C" {
  const shape::ComponentMeta& get_component_shape__TraceFormatService(unsigned long* compiler, size_t* typehash);
  const shape::ComponentMeta& get_component_shape__TraceFileService(unsigned long* compiler, size_t* typehash);
  const shape::ComponentMeta& get_component_shape__LauncherService(unsigned long* compiler, size_t* typehash);
  const shape::ComponentMeta& get_component_shape__ConfigurationService(unsigned long* compiler, size_t* typehash);
}

void staticInit()
{
  void* component = nullptr;
  unsigned long compiler = 0;
  size_t hashcode = 0;
  unsigned long expectedCompiler = (unsigned long)SHAPE_PREDEF_COMPILER;
  size_t expectedHashcode = std::type_index(typeid(shape::ComponentMeta)).hash_code();

  // get components
  shape::ComponentMap::get().setComponent(&get_component_shape__TraceFormatService(&compiler, &hashcode));
  shape::ComponentMap::get().setComponent(&get_component_shape__TraceFileService(&compiler, &hashcode));
  shape::ComponentMap::get().setComponent(&get_component_shape__LauncherService(&compiler, &hashcode));
  const shape::ComponentMeta& get_component_shape__ConfigurationService(unsigned long* compiler, size_t* typehash);
}

#else
extern "C" {
  const shape::ComponentMeta& get_component_shape__LauncherService(unsigned long* compiler, size_t* typehash);
  const shape::ComponentMeta& get_component_shape__ConfigurationService(unsigned long* compiler, size_t* typehash);
}

void staticInit()
{
  void* component = nullptr;
  unsigned long compiler = 0;
  size_t hashcode = 0;
  unsigned long expectedCompiler = (unsigned long)SHAPE_PREDEF_COMPILER;
  size_t expectedHashcode = std::type_index(typeid(shape::ComponentMeta)).hash_code();

  shape::ComponentMap::get().setComponent(&get_component_shape__LauncherService(&compiler, &hashcode));
  shape::ComponentMap::get().setComponent(&get_component_shape__ConfigurationService(&compiler, &hashcode));
}
#endif
