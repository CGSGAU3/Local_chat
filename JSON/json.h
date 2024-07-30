#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <variant>

#define SkipSpaces(File, x) \
  do                        \
    File.read(&x, 1);       \
  while (File && isspace((unsigned char)x) || x == 0)

#define ReadWhileNot(File, x, ch, Action) \
  do                                      \
  {                                       \
    File.read(&x, 1);                     \
    Action;                               \
  }                                       \
  while (File && x != ch)

class Json;
class JsonArray;
typedef std::variant<bool, double, std::string, std::nullptr_t, Json, JsonArray> FieldType;

class JsonArray
{
  friend class Json;

private:
  std::vector<FieldType> arr;

  void parse( std::istream &istr );
  std::string stringify( void ) const;

public:

  JsonArray( void ) = default;

  JsonArray( const JsonArray & ) = default;
  JsonArray & operator =( const JsonArray & ) = default;

  JsonArray( JsonArray && ) = default;
  JsonArray & operator =( JsonArray && ) = default;

  ~JsonArray( void ) = default;

  template<typename T>
  bool at( const size_t index, T &data );
};

class Json
{
  friend class JsonArray;

private:
  std::map<std::string, FieldType> obj;

  void parse( std::istream &istr, bool isRec = false );

public:

  Json( void ) = default;
  Json( const std::string &str )
  {
    parse(str);
  }

  Json( const Json & ) = default;
  Json & operator =( const Json & ) = default;

  Json( Json && ) = default;
  Json & operator =( Json && ) = default;

  ~Json( void ) = default;

  void parse( const std::string &str );
  std::string stringify( void ) const;

  FieldType & operator []( const std::string &key );

  template<typename T>
  bool at( const std::string &key, T &data );
};









