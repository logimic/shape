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

#include <vector>
#include <string>
#include <memory>

namespace shape {

  //------------------------------------
  class Properties
  {
  public:
    Properties()
    {}

    virtual ~Properties()
    {}

    enum class Result {
      ok,
      missing_error,
      type_error,
    };

    virtual void parseFile(const std::string& fname) = 0;
    virtual void encodeFile(const std::string& fname) const = 0;
    virtual void parseString(const std::string& str) = 0;
    virtual std::string encodeString() const = 0;
    virtual bool isSubsetOf(const Properties & props) const = 0;

    virtual Result getMemberAsBool(const std::string& name, bool & val) const = 0;
    virtual Result getMemberAsVectorBool(const std::string& name, std::vector<bool> & val) const = 0;
    virtual Result getMemberAsInt(const std::string& name, int & val) const = 0;
    virtual Result getMemberAsVectorInt(const std::string& name, std::vector<int> & val) const = 0;
    virtual Result getMemberAsDouble(const std::string& name, double & val) const = 0;
    virtual Result getMemberAsVectorDouble(const std::string& name, std::vector<double> & val) const = 0;
    virtual Result getMemberAsString(const std::string& name, std::string & val) const = 0;
    virtual Result getMemberAsVectorString(const std::string& name, std::vector<std::string> & val) const = 0;
    virtual Result getMemberAsProperties(const std::string& name, std::unique_ptr<Properties> & val) const = 0;
    virtual Result getMemberAsVectorProperties(const std::string& name, std::vector<std::unique_ptr<Properties>> & val) const = 0;

    //TODO setters for all
    virtual Result setMemberAsString(const std::string& name, const std::string & val) = 0;

  };
}
