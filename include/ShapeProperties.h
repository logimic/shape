#pragma once

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
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

    virtual const rapidjson::Document& getAsJson() const = 0;

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

    virtual Result setMemberAsBool(const std::string& name, const bool & val) = 0;
    virtual Result setMemberAsVectorBool(const std::string& name, const std::vector<bool> & val) = 0;
    virtual Result setMemberAsInt(const std::string& name, const int & val) = 0;
    virtual Result setMemberAsVectorInt(const std::string& name, const std::vector<int> & val) = 0;
    virtual Result setMemberAsDouble(const std::string& name, const double & val) = 0;
    virtual Result setMemberAsVectorDouble(const std::string& name, const std::vector<double> & val) = 0;
    virtual Result setMemberAsString(const std::string& name, const std::string & val) = 0;
    virtual Result setMemberAsVectorString(const std::string& name, const std::vector<std::string> & val) = 0;

    //TODO
    //virtual Result setMemberAsProperties(const std::string& name, std::unique_ptr<Properties> & val) const = 0;
    //virtual Result setMemberAsVectorProperties(const std::string& name, std::vector<std::unique_ptr<Properties>> & val) const = 0;

  };
}
