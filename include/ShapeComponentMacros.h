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

#define COMPONENT_DECLARATION_INIT(namespace__comp) \
extern "C" { \
  SHAPE_ABI_EXPORT void* get_component_##namespace__comp(unsigned long* compiler, unsigned long* typehash) \
  { \
    *compiler = SHAPE_PREDEF_COMPILER; \
    *typehash = std::type_index(typeid(shape::ComponentMeta)).hash_code();

#define COMPONENT_DECLARATION(comp) \
    static shape::ComponentMetaTemplate<comp> component(#comp);

#define COMPONENT_PROVIDE_INTERFACE(iface) \
    component.provideInterface<iface>(#iface);

#define COMPONENT_REQUIRE_INTERFACE(iface, mandatory, multiple) \
    component.requireInterface<iface>(#iface, shape::Optionality::##mandatory, shape::Cardinality::##multiple);

#define COMPONENT_DECLARATION_FINIT() \
    return &component; \
  } \
}
