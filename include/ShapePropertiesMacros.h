#pragma once

#include "Trace.h"
#include "ShapeProperties.h"

//  Possible type: Bool, Int, Double, String, Properties

#define GET_MEMBER_AS(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).getMemberAs##type(name, value); \
  if (shape::Properties::Result::missing_error == ret) \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\" not found") \
  else if (shape::Properties::Result::type_error == ret) \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't type: " << #type) \
}

#define GET_MEMBER_AS_VECTOR(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).getMemberAsVector##type(name, value); \
  if (shape::Properties::Result::missing_error == ret) \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\" not found") \
  else if (shape::Properties::Result::type_error == ret) \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't vector of type: " << #type) \
}

#define GET_POSSIBLE_MEMBER_AS(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).getMemberAs##type(name, value); \
  if (shape::Properties::Result::missing_error == ret) \
    TRC_INFORMATION("Property \"" << path << "." << (name) << "\" not found") \
  else if (shape::Properties::Result::type_error == ret) \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't type: " << #type) \
}

#define GET_POSSIBLE_MEMBER_AS_VECTOR(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).getMemberAsVector##type(name, value); \
  if (shape::Properties::Result::missing_error == ret) \
    TRC_INFORMATION("Property \"" << path << "." << (name) << "\" not found") \
  else if (shape::Properties::Result::type_error == ret) \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't vector of type: " << #type) \
}

#define SET_MEMBER_AS(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).setMemberAs##type(name, value); \
  if (shape::Properties::Result::type_error == ret) \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't type: " << #type) \
}
