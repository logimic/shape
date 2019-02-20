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

#include "Trace.h"
#include "ShapeProperties.h"

//  Possible type: Bool, Int, Double, String, Properties

#define GET_MEMBER_AS(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).getMemberAs##type(name, value); \
  if (shape::Properties::Result::missing_error == ret) { \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\" not found") \
  } else if (shape::Properties::Result::type_error == ret) { \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't type: " << #type) } \
}

#define GET_MEMBER_AS_VECTOR(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).getMemberAsVector##type(name, value); \
  if (shape::Properties::Result::missing_error == ret) { \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\" not found") \
  } else if (shape::Properties::Result::type_error == ret) { \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't vector of type: " << #type) } \
}

#define GET_POSSIBLE_MEMBER_AS(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).getMemberAs##type(name, value); \
  if (shape::Properties::Result::missing_error == ret) { \
    TRC_INFORMATION("Property \"" << path << "." << (name) << "\" not found") \
  } else if (shape::Properties::Result::type_error == ret) { \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't type: " << #type) }\
}

#define GET_POSSIBLE_MEMBER_AS_VECTOR(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).getMemberAsVector##type(name, value); \
  if (shape::Properties::Result::missing_error == ret) { \
    TRC_INFORMATION("Property \"" << path << "." << (name) << "\" not found") \
  } else if (shape::Properties::Result::type_error == ret) { \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't vector of type: " << #type) } \
}

#define SET_MEMBER_AS(properties, type, name, path, value) \
{ \
  shape::Properties::Result ret = (properties).setMemberAs##type(name, value); \
  if (shape::Properties::Result::type_error == ret) { \
    THROW_EXC_TRC_WAR(std::logic_error, "Property \"" << path << "." << (name) << "\"  isn't type: " << #type) } \
}
