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

#include "ShapeProperties.h"
#include "ShapePropertiesMacros.h"
#include "Trace.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include <fstream>
#include <vector>
#include <utility>
#include <stdexcept>

namespace shape {
  static const std::string CONFIG_COMPONENT_STR("component");
  static const std::string CONFIG_INSTANCE_STR("instance");

  //------------------------------------
  class PropertiesJson : public Properties
  {
  public:
    PropertiesJson()
    {}

    PropertiesJson(const PropertiesJson& props)
    {
      m_props.CopyFrom(props.m_props, m_props.GetAllocator());
    }

    PropertiesJson& operator= (const PropertiesJson& prop)
    {
      m_props.CopyFrom(prop.m_props, m_props.GetAllocator());
      return *this;
    }

    void parseFile(const std::string& fname) override
    {
      std::ifstream ifs(fname);
      if (!ifs.is_open()) {
        THROW_EXC_TRC_WAR(std::logic_error, "Cannot open: " << PAR(fname));
      }

      rapidjson::IStreamWrapper isw(ifs);
      m_props.ParseStream(isw);

      if (m_props.HasParseError()) {
        THROW_EXC_TRC_WAR(std::logic_error, "Json parse error: " << PAR(fname) << NAME_PAR(emsg, m_props.GetParseError()) <<
          NAME_PAR(eoffset, m_props.GetErrorOffset()));
      }
    }

    void encodeFile(const std::string& fname) const override
    {
      //TODO check buf size, dynamic alloc
      const size_t BUFFER_SIZE = 65536u;

      FILE * pFile;
      pFile = fopen(fname.c_str(), "w");
      if (!pFile) {
        THROW_EXC_TRC_WAR(std::logic_error, "Cannot open: " << PAR(fname));
      }

      std::vector<char> fileBuffer(BUFFER_SIZE);
      rapidjson::FileWriteStream f(pFile, fileBuffer.data(), fileBuffer.size());
      rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(f);
      m_props.Accept(writer);
      fclose(pFile);
    }

    void parseString(const std::string& str) override
    {
      //TODO
    }

    bool isSubsetOf(const Properties & props) const
    {
      bool result = true;
      try {
        for (auto iter = m_props.MemberBegin(); iter != m_props.MemberEnd(); iter++) {
          if (iter->value.IsBool()) {
            bool value;
            GET_MEMBER_AS(props, Bool, iter->name.GetString(), "", value);
            if (value != iter->value.GetBool()) {
              result = false;
              break;
            }
          }
          else if (iter->value.IsInt()) {
            int value;
            GET_MEMBER_AS(props, Int, iter->name.GetString(), "", value);
            if (value != iter->value.GetInt()) {
              result = false;
              break;
            }
          }
          else if (iter->value.IsDouble()) {
            double value;
            GET_MEMBER_AS(props, Double, iter->name.GetString(), "", value);
            if (value != iter->value.GetDouble()) {
              result = false;
              break;
            }
          }
          else if (iter->value.IsString()) {
            std::string value;
            GET_MEMBER_AS(props, String, iter->name.GetString(), "", value);
            if (value != iter->value.GetString()) {
              result = false;
              break;
            }
          }
        }
      }
      catch (std::exception & e) {
        CATCH_EXC_TRC_WAR(std::exception, e, "Comparison failed");
        result = false;
      }
      return result;
    }

    std::string encodeString() const override
    {
      rapidjson::StringBuffer buffer;
      rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
      m_props.Accept(writer);
      return buffer.GetString();
    }

    Result getMemberAsBool(const std::string& name, bool & val) const override
    {
      return getMemberAs(name, val);
    }

    Result getMemberAsVectorBool(const std::string& name, std::vector<bool> & val) const override
    {
      return getMemberAsVector(name, val);
    }

    Result getMemberAsInt(const std::string& name, int & val) const override
    {
      return getMemberAs(name, val);
    }

    Result getMemberAsVectorInt(const std::string& name, std::vector<int> & val) const override
    {
      return getMemberAsVector(name, val);
    }

    Result getMemberAsDouble(const std::string& name, double & val) const override
    {
      return getMemberAs(name, val);
    }

    Result getMemberAsVectorDouble(const std::string& name, std::vector<double> & val) const override
    {
      return getMemberAsVector(name, val);
    }

    Result getMemberAsString(const std::string& name, std::string & val) const override
    {
      Result retval = Result::missing_error;
      const auto & m = m_props.FindMember(name.c_str());
      if (m != m_props.MemberEnd()) {
        if (!m->value.IsString()) {
          TRC_WARNING("Expected: " << typeid(std::string).name() << ", detected: " << PAR(name) << NAME_PAR(type, m->value.GetType()));
          retval = Result::type_error;
        }
        val = m->value.GetString();
        retval = Result::ok;
      }
      return retval;
    }

    Result getMemberAsVectorString(const std::string& name, std::vector<std::string> & val) const override
    {
      Result retval = Result::missing_error;
      const auto & m = m_props.FindMember(name.c_str());
      if (m != m_props.MemberEnd()) {
        const rapidjson::Value& vct = m->value;
        if (!vct.IsArray()) {
          TRC_WARNING("Expected: Json Array, detected: " << PAR(name) << NAME_PAR(type, vct.GetType()));
          retval = Result::type_error;
        }
        else {
          for (auto itr = vct.Begin(); itr != vct.End(); ++itr) {
            if (!itr->IsString()) {
              TRC_WARNING("Expected: " << typeid(std::string).name() << ", detected: " << PAR(name) << NAME_PAR(type, itr->GetType()));
              retval = Result::type_error;
              break;
            }
            val.push_back(itr->GetString());
          }
          retval = Result::ok;
        }
      }
      return retval;
    }

    Result getMemberAsProperties(const std::string& name, std::unique_ptr<Properties> & val) const override
    {
      Result retval = Result::missing_error;
      const auto & m = m_props.FindMember(name.c_str());
      if (m != m_props.MemberEnd()) {
        if (!m->value.IsObject()) {
          TRC_WARNING("Expected: Json Object, detected: " << PAR(name) << NAME_PAR(type, m->value.GetType()));
          retval = Result::type_error;
        }
        PropertiesJson *props = shape_new PropertiesJson;
        props->m_props.CopyFrom(m->value, props->m_props.GetAllocator());
        val.reset(props);
        retval = Result::ok;
      }
      return retval;
    }

    Result getMemberAsVectorProperties(const std::string& name, std::vector<std::unique_ptr<Properties>> & val) const override
    {
      Result retval = Result::missing_error;
      const auto & m = m_props.FindMember(name.c_str());
      if (m != m_props.MemberEnd()) {
        const rapidjson::Value& vct = m->value;
        if (!vct.IsArray()) {
          TRC_WARNING("Expected: Json Array, detected: " << PAR(name) << NAME_PAR(type, vct.GetType()));
          retval = Result::type_error;
        }
        else {
          for (auto itr = vct.Begin(); itr != vct.End(); ++itr) {
            if (!itr->IsObject()) {
              TRC_WARNING("Expected: Json Object, detected: " << PAR(name) << NAME_PAR(type, itr->GetType()));
              retval = Result::type_error;
              break;
            }
            PropertiesJson *props = shape_new PropertiesJson;
            props->m_props.CopyFrom(*itr, props->m_props.GetAllocator());
            val.push_back(std::unique_ptr<Properties>(props));
          }
          retval = Result::ok;
        }
      }
      return retval;
    }

    Result setMemberAsString(const std::string& name, const std::string & val) override
    {
      Result retval = Result::ok;
      rapidjson::Document::AllocatorType& alloc = m_props.GetAllocator();
      auto m = m_props.FindMember(name.c_str());
      if (m == m_props.MemberEnd()) {
        rapidjson::Value v;
        v.SetString(val.c_str(), alloc);
        rapidjson::Value jname(rapidjson::kStringType);
        jname.SetString(name.c_str(), alloc);
        m_props.AddMember(jname, v, alloc);
      }
      if (!m->value.IsString()) {
        TRC_WARNING("Expected: " << typeid(std::string).name() << ", detected: " << PAR(name) << NAME_PAR(type, m->value.GetType()));
        retval = Result::type_error;
      }
      else {
        m->value.SetString(val.c_str(), alloc);
      }
      return retval;
    }

 private:
    template<typename T>
    Result getMemberAs(const std::string& name, T & val) const
    {
      Result retval = Result::missing_error;
      const auto & m = m_props.FindMember(name.c_str());
      if (m != m_props.MemberEnd()) {
        if (!m->value.Is<T>()) {
          TRC_WARNING("Expected: " << typeid(T).name() << ", detected: " << PAR(name) << NAME_PAR(type, m->value.GetType()));
          retval = Result::type_error;
        }
        val = m->value.Get<T>();
        retval = Result::ok;
      }
      return retval;
    }

    template<typename T>
    Result getMemberAsVector(const std::string& name, std::vector<T> & val) const
    {
      Result retval = Result::missing_error;
      const auto & m = m_props.FindMember(name.c_str());
      if (m != m_props.MemberEnd()) {
        const rapidjson::Value& vct = m->value;
        if (!vct.IsArray()) {
          TRC_WARNING("Expected: Json Array, detected: " << PAR(name) << NAME_PAR(type, vct.GetType()));
          retval = Result::type_error;
        }
        else {
          for (auto itr = vct.Begin(); itr != vct.End(); ++itr) {
            if (!itr->Is<T>()) {
              TRC_WARNING("Expected: " << typeid(T).name() << ", detected: " << PAR(name) << NAME_PAR(type, itr->GetType()));
              retval = Result::type_error;
              break;
            }
            val.push_back(itr->Get<T>());
          }
          retval = Result::ok;
        }
      }
      return retval;
    }

  private:
    rapidjson::Document m_props;
  };
}
