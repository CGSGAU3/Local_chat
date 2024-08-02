#pragma once

#include <vector>
#include <string>
#include <memory>

#include "macro.h"

class FieldInfo;

class TypeInfo
{
public:
  std::string name;
  std::vector<FieldInfo> fields;
};

class FieldInfo
{
public:
  std::string name;
  int offset;
  std::string type;

  void setName( const char *fieldName )
  {
    while (fieldName[0] == ' ')
      fieldName++;
    name = fieldName;
  }
};

#define PUSH_FIELD_INFO(x)                        \
  fi.setName(ARGNAME_AS_STRING(x));               \
  fi.offset = offsetof(_className, ARGNAME(x));   \
  fi.type = typeid(ARGTYPE(x)).name();            \
  t.fields.push_back(fi);


#define JSON_FIELDS(className, ...)                             \
    /* Dump field types and names */                            \
    DOFOREACH_SEMICOLON(ARGPAIR, __VA_ARGS__)                   \
    /* typedef is accessable from PUSH_FIELD_INFO define */     \
    typedef className _className;                               \
                                                                \
    static TypeInfo & getType()                                 \
    {                                                           \
      static TypeInfo t;                                        \
                                                                \
      if (t.name.length())                                      \
        return t;                                               \
      t.name = #className;                                      \
      FieldInfo fi;                                             \
      /* Dump offsets and field names */                        \
      DOFOREACH_SEMICOLON(PUSH_FIELD_INFO, __VA_ARGS__);        \
      return t;                                                 \
    }