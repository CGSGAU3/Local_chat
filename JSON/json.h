#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cassert>

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
using FieldType = std::variant<bool, double, std::string, std::nullptr_t, Json, JsonArray>;

namespace std
{
  inline string to_string( const ::FieldType &ft );
}

class JsonArray
{
  friend class Json;
  friend std::string std::to_string( const FieldType &ft );

private:
  std::vector<FieldType> arr;

  void save( std::ostream &istr, int prec = 0 ) const;
  void saveRaw( std::ostream &istr ) const;

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
  bool at( const size_t index, T &data )
  {
    if (index >= arr.size())
      return false;

    try
    {
      if constexpr (std::is_arithmetic_v<T>)
        data = (T)std::get<double>(arr[index]);
      else
        data = std::get<T>(arr[index]);

      return true;
    }
    catch (...)
    {
      return false;
    }
  }
};

class Json
{
  friend class JsonArray;

private:
  std::unordered_map<std::string, FieldType> obj;

  void parse( std::istream &istr, bool isRec = false );
  void save( std::ostream &ostr, int prec = 0 ) const;
  void saveRaw( std::ostream &ostr ) const;

  class ARef;

  class Ref
  {
  private:
    Json *ptr;
    FieldType &val;
    const std::string &key;

  public:

    Ref( Json &j, FieldType &ft, const std::string &k ) : ptr(&j), val(ft), key(k)
    {
    }

    template<typename T>
    explicit operator T( void ) const
    {
      if (std::is_same_v<T, bool> && val.index() == 0)
        return std::get<T>(val);
      if (std::is_arithmetic_v<T> && val.index() == 1)
        return (T)std::get<double>(val);
      throw std::bad_cast();
    }

    operator std::string( void ) const
    {
      if (val.index() == 2)
        return std::get<std::string>(val);
      throw std::bad_cast();
    }

    operator nullptr_t( void ) const
    {
      if (val.index() == 3)
        return std::get<nullptr_t>(val);
      throw std::bad_cast();
    }

    operator Json( void ) const
    {
      if (val.index() == 4)
        return std::get<Json>(val);
      throw std::bad_cast();
    }

    operator JsonArray( void ) const
    {
      if (val.index() == 5)
        return std::get<JsonArray>(val);
      throw std::bad_cast();
    }

    template<typename T>
    Ref & operator =( const T &any )
    {
      FieldType test;
      double numAny;

      // Validate types
      if constexpr (std::is_arithmetic_v<T>)
      {
        numAny = (double)any;
        test = numAny;
        ptr->obj[key] = numAny;
      }
      else
      {
        test = any;
        ptr->obj[key] = any;
      }

      return *this;
    }

    Ref operator []( const std::string &kkey )
    {
      assert(val.index() == 4);
      return Ref(std::get<Json>(val), std::get<Json>(val).obj[kkey], kkey);
    }

    ARef operator []( const size_t idx )
    {
      assert(val.index() == 5);
      return ARef(std::get<JsonArray>(val), std::get<JsonArray>(val).arr[idx], idx);
    }

  };

  class ARef
  {
  private:
    JsonArray *ptr;
    FieldType &val;
    size_t index;

  public:

    ARef( JsonArray &ja, FieldType &ft, const size_t idx ) : ptr(&ja), val(ft), index(idx)
    {
    }

    template<typename T>
    explicit operator T( void ) const
    {
      if (std::is_same_v<T, bool> && val.index() == 0)
        return std::get<T>(val);
      if (std::is_arithmetic_v<T> && val.index() == 1)
        return (T)std::get<double>(val);
    }

    operator std::string( void ) const
    {
      if (val.index() == 2)
        return std::get<std::string>(val);
      throw std::bad_cast();
    }

    operator nullptr_t( void ) const
    {
      if (val.index() == 3)
        return std::get<nullptr_t>(val);
      throw std::bad_cast();
    }

    operator Json( void ) const
    {
      if (val.index() == 4)
        return std::get<Json>(val);
      throw std::bad_cast();
    }

    operator JsonArray( void ) const
    {
      if (val.index() == 5)
        return std::get<JsonArray>(val);
      throw std::bad_cast();
    }

    template<typename T>
    ARef & operator =( const T &any )
    {
      FieldType test;
      double numAny;

      // Validate types
      if constexpr (std::is_arithmetic_v<T>)
      {
        numAny = (double)any;
        test = numAny;
        ptr->arr[index] = numAny;
      }
      else
      {
        test = any;
        ptr->arr[index] = any;
      }

      return *this;
    }

    Ref operator []( const std::string &kkey )
    {
      assert(val.index() == 4);
      return Json::Ref(std::get<Json>(val), std::get<Json>(val).obj[kkey], kkey);
    }

    ARef operator []( const size_t idx )
    {
      assert(val.index() == 5);
      return ARef(std::get<JsonArray>(val), std::get<JsonArray>(val).arr[idx], idx);
    }
  };

public:

  Json( void ) = default;
  Json( const std::string &str )
  {
    parse(str);
  }

  Json( const Json & ) = default;
  Json & operator =( const Json & ) = default;

  Ref operator []( const std::string &key )
  {
    if (obj.find(key) == obj.end())
      obj[key] = Json();
    return Ref(*this, obj[key], key);
  }

  Json( Json && ) = default;
  Json & operator =( Json && ) = default;

  ~Json( void ) = default;

  void parse( const std::string &str );
  void parseFile( const std::string &filename );
  std::string stringify( void ) const;

  template<typename T>
  bool at( const std::string &key, T &data )
  {
    if (obj.find(key) == obj.end())
    return false;

    try
    {
      if constexpr (std::is_arithmetic_v<T>)
        data = (T)std::get<double>(obj[key]);
      else
        data = std::get<T>(obj[key]);
      return true;
    }
    catch (...)
    {
      return false;
    }
  }
};

namespace std
{
  inline string to_string( const ::FieldType &ft )
  {
    switch (ft.index())
    {
    case 0: // bool
      if (get<bool>(ft) == true)
        return "true";
      else
        return "false";
    case 1: // double
      return to_string(get<double>(ft));
    case 2: // string
      return "\"" + get<std::string>(ft) + "\"";
    case 3: // null
      return "null";
    case 4: // Json
      return get<Json>(ft).stringify();
    case 5: // JsonArray
      return get<JsonArray>(ft).stringify();
    default:
      throw std::exception("WTH! FieldType contain only 6 types!");
    }
  }
}