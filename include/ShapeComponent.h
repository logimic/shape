#pragma once

#include "ShapeDefines.h"
#include "ShapeProperties.h"
#include "ShapeDefines.h"
#include <string>
#include <typeindex>
#include <exception>
#include <map>

namespace shape {
  
  enum class Optionality {
    UNREQUIRED,
    MANDATORY
  };

  enum class Cardinality {
    SINGLE,
    MULTIPLE
  };

  //-------------------------------------
  class ObjectTypeInfo
  {
  public:
    ObjectTypeInfo() = delete;
    ObjectTypeInfo(std::string name, std::type_index typeIndex, void* object)
      :m_name(name)
      ,m_typeIndex(typeIndex)
      ,m_object(object)
    {}
    const std::string& getName() const { return m_name; }
    const std::type_index& getTypeIndex() const { return m_typeIndex; }
    void* getObject() const { return m_object; }

    template<class ImplementationClass>
    ImplementationClass* typed_ptr() const
    {
      if (m_typeIndex == std::type_index(typeid(ImplementationClass))) {
        return static_cast<ImplementationClass*>(m_object);
      }
      else {
        //TODO provide type details
        throw std::logic_error("type error");
      }
    }
    
    bool operator == (const ObjectTypeInfo& other) const
    {
      return m_object == other.m_object;
    }

  private:
    std::string m_name;
    std::type_index m_typeIndex;
    void* m_object;
  };

  //-------------------------------------
  class ProvidedInterfaceMeta
  {
  public:
    ProvidedInterfaceMeta() = delete;
    ProvidedInterfaceMeta(const std::string& providerName, const std::string& ifaceName)
      :m_providerName(providerName)
      , m_ifaceName(ifaceName)
    {}
    const std::string& getProviderName() const { return m_providerName; }
    const std::string& getInterfaceName() const { return m_ifaceName; }
    virtual const std::type_index&  getProviderTypeIndex() const = 0;
    virtual const std::type_index&  getInterfaceTypeIndex() const = 0;
    virtual ObjectTypeInfo getAsInterface(ObjectTypeInfo* object) const = 0;

  private:
    std::string m_providerName;
    std::string m_ifaceName;
  };

  //-------------------------------------
  class RequiredInterfaceMeta
  {
  public:
    RequiredInterfaceMeta() = delete;
    RequiredInterfaceMeta(const std::string& ifaceName, Optionality optionality, Cardinality cardinality)
      :m_ifaceName(ifaceName)
      ,m_optionality(optionality)
      ,m_cardinality(cardinality)
    {}
    virtual void attachInterface(const ObjectTypeInfo* object, const ObjectTypeInfo* iface) const = 0;
    virtual void detachInterface(const ObjectTypeInfo* object, const ObjectTypeInfo* iface) const = 0;
    const std::string& getRequirerName() const { return m_requirerName; }
    const std::string& getInterfaceName() const { return m_ifaceName; }
    virtual const std::type_index&  getRequirerTypeIndex() const = 0;
    virtual const std::type_index&  getInterfaceTypeIndex() const = 0;
    //TODO former ComponentReference
    //enum Policy {
    //  STATIC,
    //  DYNAMIC
    //};
    bool getPolicyStatic() const { return m_policyStatic; }
    Optionality getOptionality() const { return m_optionality; }
    Cardinality getCardinality() const { return m_cardinality; }

  private:
    std::string m_requirerName;
    std::string m_ifaceName;
    Optionality m_optionality = Optionality::MANDATORY;
    Cardinality m_cardinality = Cardinality::MULTIPLE;
    bool m_policyStatic = true;
  };

  //-------------------------------------
  class ComponentMeta
  {
  public:
    ComponentMeta() = delete;
    ComponentMeta(const std::string& componentName)
      :m_componentName(componentName) {}
    
    const std::string& getComponentName() const { return m_componentName; }
    unsigned getCompilerVersion() const { return SHAPE_PREDEF_COMPILER; }

    virtual ObjectTypeInfo* create() const = 0;
    virtual void destroy(ObjectTypeInfo* object) const = 0;
    virtual void activate(ObjectTypeInfo* object, const Properties* props) const = 0;
    virtual void deactivate(ObjectTypeInfo* object) const = 0;
    virtual void modify(ObjectTypeInfo* object, const Properties* props) const = 0;

    const std::map<std::string, const ProvidedInterfaceMeta*> &  getProvidedInterfaceMap() const
    {
      return m_providedInterfaceMap;
    }
    
    const ProvidedInterfaceMeta*  getProvidedInterface(const std::string& ifaceName) const
    {
      const ProvidedInterfaceMeta* retval = nullptr;
      auto found = m_providedInterfaceMap.find(ifaceName);
      if (found != m_providedInterfaceMap.end())
        retval = found->second;
      return retval;
    }

    const std::map<std::string, const RequiredInterfaceMeta*> &  getRequiredInterfaceMap() const
    { 
      return m_requiredInterfaceMap;
    }

    const RequiredInterfaceMeta* getRequiredInterface(const std::string& ifaceName) const
    {
      const RequiredInterfaceMeta* ret = nullptr;
      auto found = m_requiredInterfaceMap.find(ifaceName);
      if (found != m_requiredInterfaceMap.end())
        ret = found->second;
      return ret;
    }

  protected:
    std::map<std::string, const ProvidedInterfaceMeta* > m_providedInterfaceMap;
    std::map<std::string, const RequiredInterfaceMeta* > m_requiredInterfaceMap;
  private:
    std::string m_componentName;
  };

  //-------------------------------------
  template <class ImplementationClass, class ProvidedInterfaceClass>
  class ProvidedInterfaceMetaTemplate: public ProvidedInterfaceMeta
  {
  public:
    ProvidedInterfaceMetaTemplate() = delete;
    ProvidedInterfaceMetaTemplate(const std::string& providerName, const std::string& interfaceName)
      :ProvidedInterfaceMeta(providerName, interfaceName)
      , m_providerTypeIndex(typeid(ImplementationClass))
      , m_interfaceTypeIndex(typeid(ProvidedInterfaceClass))
    {}

    const std::type_index&  getProviderTypeIndex() const override
    {
      return m_providerTypeIndex;
    }

    const std::type_index&  getInterfaceTypeIndex() const override
    {
      return m_interfaceTypeIndex;
    }

    ObjectTypeInfo getAsInterface(ObjectTypeInfo* object) const override
    {
      ImplementationClass* componentImpl = object->typed_ptr<ImplementationClass>();
      ProvidedInterfaceClass* providedInterface = static_cast<ProvidedInterfaceClass*> (componentImpl);
      ObjectTypeInfo retval = { getInterfaceName(), std::type_index(typeid(ProvidedInterfaceClass)), providedInterface };
      return retval;
    }

  private:
    std::type_index m_providerTypeIndex;
    std::type_index m_interfaceTypeIndex;
  };

  //-------------------------------------
  template <class ImplementationClass, class RequiredInterfaceClass>
  class RequiredInterfaceMetaTemplate: public RequiredInterfaceMeta
  {
  public:
    RequiredInterfaceMetaTemplate() = delete;
    RequiredInterfaceMetaTemplate(std::string name, Optionality optionality, Cardinality cardinality)
      :RequiredInterfaceMeta(name, optionality, cardinality)
      , m_requiredInterfaceTypeIndex(typeid(RequiredInterfaceClass))
      , m_requirerTypeIndex(typeid(ImplementationClass))
    {}

    void attachInterface(const ObjectTypeInfo* object, const ObjectTypeInfo* iface) const override
    {
      attachInterface(object->typed_ptr<ImplementationClass>(), iface->typed_ptr<RequiredInterfaceClass>());
    }
    
    void detachInterface(const ObjectTypeInfo* object, const ObjectTypeInfo* iface) const override
    {
      detachInterface(object->typed_ptr<ImplementationClass>(), iface->typed_ptr<RequiredInterfaceClass>());
    }

    const std::type_index&  getRequirerTypeIndex() const override
    {
      return m_requirerTypeIndex;
    }

    const std::type_index&  getInterfaceTypeIndex() const override
    {
      return m_requiredInterfaceTypeIndex;
    }

  private:
    void attachInterface(ImplementationClass* object, RequiredInterfaceClass* iface) const
    {
      object->attachInterface(iface);
    }

    void detachInterface(ImplementationClass* object, RequiredInterfaceClass* iface) const
    {
      object->detachInterface(iface);
    }

    std::type_index m_requiredInterfaceTypeIndex;
    std::type_index m_requirerTypeIndex;

  };

  //-------------------------------------
  template<class ImplementationClass>
  class ComponentMetaTemplate : public ComponentMeta
  {
  public:
    ComponentMetaTemplate() = delete;
    ComponentMetaTemplate(std::string componentName)
      :ComponentMeta(componentName) {}

    ObjectTypeInfo* create() const override
    {
      ObjectTypeInfo* ret = 
        shape_new ObjectTypeInfo(getComponentName(), std::type_index(typeid(ImplementationClass)), shape_new ImplementationClass);
      return ret;
    }

    void destroy(ObjectTypeInfo* object) const override
    {
      delete object->typed_ptr<ImplementationClass>();
      delete object;
    }

    void activate(ObjectTypeInfo* object, const Properties* props) const override
    {
      object->typed_ptr<ImplementationClass>()->activate(props);
    }

    void deactivate(ObjectTypeInfo* object) const override
    {
      object->typed_ptr<ImplementationClass>()->deactivate();
    }

    void modify(ObjectTypeInfo* object, const Properties* props) const override
    {
      object->typed_ptr<ImplementationClass>()->modify(props);
    }

    template<class ProvidedInterfaceClass>
    void provideInterface(const std::string& interfaceName)
    {
      static shape::ProvidedInterfaceMetaTemplate<ImplementationClass, ProvidedInterfaceClass> providedInterface(getComponentName(), interfaceName);
      auto result = m_providedInterfaceMap.insert(std::make_pair(interfaceName, &providedInterface));
      if (!result.second) {
        throw std::logic_error("provided interface duplicity");
      }
    }

    template<class RequiredInterfaceClass>
    void requireInterface(const std::string& interfaceName, Optionality optionality, Cardinality cardinality)
    {
      static shape::RequiredInterfaceMetaTemplate<ImplementationClass, RequiredInterfaceClass> requiredInterface(interfaceName, optionality, cardinality);
      auto result = m_requiredInterfaceMap.insert(std::make_pair(requiredInterface.getInterfaceName(), &requiredInterface));
      if (!result.second) {
        throw std::logic_error("required interface duplicity");
      }
    }
  };

  //--------------------------------------
  typedef void*(*GetMetaComponent)(unsigned long*, unsigned long*);

}
