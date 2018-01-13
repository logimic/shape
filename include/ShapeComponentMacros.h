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
