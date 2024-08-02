#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cassert>

#include "reflect.h"

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

class Json
{
public:
  class Array;
  using FieldType = std::variant<bool, double, std::string, std::nullptr_t, Json, Json::Array>;

private:
  std::unordered_map<std::string, FieldType> obj;

  void parse( std::istream &istr, bool isRec = false );
  void save( std::ostream &ostr, int prec = 0 ) const;
  void saveRaw( std::ostream &ostr ) const;

  class Array;

  class Ref
  {
  private:
    FieldType &val;

  public:

    Ref( FieldType &ft ) : val(ft)
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

    operator Json &( void )
    {
      if (val.index() == 4)
        return std::get<Json>(val);
      throw std::bad_cast();
    }

    operator Array( void ) const
    {
      if (val.index() == 5)
        return std::get<Array>(val);
      throw std::bad_cast();
    }

    operator Array &( void )
    {
      if (val.index() == 5)
        return std::get<Array>(val);
      throw std::bad_cast();
    }

    template<typename T>
    Ref & operator =( const T &any )
    {
      double numAny;

      // Validate types
      if constexpr (std::is_arithmetic_v<T>)
      {
        numAny = (double)any;
        val = numAny;
      }
      else if constexpr (std::is_same_v<T, std::nullptr_t>)
      {
        val = nullptr;
      }
      else if constexpr (std::is_convertible_v<T, std::string>)
      {
        val = std::string(any);
      }
      else
      {
        val = any;
      }

      return *this;
    }

    Ref & operator =( const Ref &any )
    {
      switch (any.val.index())
      {
      case 0:
        *this = std::get<bool>(any.val);
        break;
      case 1:
        *this = std::get<double>(any.val);
        break;
      case 2:
        *this = std::get<std::string>(any.val);
        break;
      case 3:
        *this = std::get<std::nullptr_t>(any.val);
        break;
      case 4:
        *this = std::get<Json>(any.val);
        break;
      case 5:
        *this = std::get<Array>(any.val);
        break;
      default:
        throw std::exception("U just added new type?");

      }

      return *this;
    }

    Ref operator []( const std::string &kkey )
    {
      assert(val.index() == 4);
      return std::get<Json>(val)[kkey];
    }

    Ref operator []( const size_t idx )
    {
      assert(val.index() == 5);
      return std::get<Array>(val)[idx];
    }

    const Ref operator []( const std::string &kkey ) const
    {
      assert(val.index() == 4);
      return std::get<Json>(val)[kkey];
    }

    const Ref operator []( const size_t idx ) const
    {
      assert(val.index() == 5);
      return std::get<Array>(val)[idx];
    }

  };

public:

  class Array
  {
    friend class Ref;
    friend class Json;

  private:
    std::vector<FieldType> arr;

    void save( std::ostream &istr, int prec = 0 ) const;
    void saveRaw( std::ostream &istr ) const;

    void parse( std::istream &istr );
    std::string stringify( void ) const;

  public:

    Array( void ) = default;

    Array( const Array & ) = default;
    Array & operator =( const Array & ) = default;

    Array( Array && ) = default;
    Array & operator =( Array && ) = default;

    Array( const size_t size );

    ~Array( void ) = default;

    Ref operator []( const size_t index )
    {
      assert(index >= 0 && index <= arr.size());

      if (index == arr.size())
        add(nullptr);
      return Ref(arr[index]);
    }

    const Ref operator []( const size_t index ) const
    {
      assert(index >= 0 && index < arr.size());
      return Ref(const_cast<FieldType &>(arr[index]));
    }

    void resize( const size_t size )
    {
      arr.resize(size);
    }

    template<typename T>
    void add( const T &data )
    {
      if constexpr (std::is_arithmetic_v<T>)
        arr.push_back((double)data);
      else
      {
        FieldType valid = data;

        arr.push_back(valid);
      }
    }

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

private:

  static inline std::string to_string( const FieldType &ft )
  {
    double num;

    switch (ft.index())
    {
    case 0: // bool
      if (std::get<bool>(ft) == true)
        return "true";
      else
        return "false";
    case 1: // double
      num = std::get<double>(ft);
      if (fabs(num - (long long)num) < 1e-5)
        return std::to_string((long long)num);
      return std::to_string(num);
    case 2: // string
      return "\"" + std::get<std::string>(ft) + "\"";
    case 3: // null
      return "null";
    case 4: // Json
      return std::get<Json>(ft).stringify();
    case 5: // Array
      return std::get<Array>(ft).stringify();
    default:
      throw std::exception("WTH! FieldType contain only 6 types!");
    }
  }

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
    return Ref(obj[key]);
  }

  const Ref operator []( const std::string &key ) const
  {
    return Ref(const_cast<FieldType &>(obj.at(key)));
  }

  Json( Json && ) = default;
  Json & operator =( Json && ) = default;

  ~Json( void ) = default;

  void parse( const std::string &str );
  void parseFile( const std::string &filename );
  std::string stringify( void ) const;
  void saveToFile( const std::string &filename );

  /******  MANAGEMENT  ******/

  bool find( const std::string &key )
  {
    return obj.find(key) != obj.end();
  }

  void remove( const std::string &key )
  {
    if (find(key))
      obj.erase(key);
  }

  template<typename T>
  bool at( const std::string &key, T &data )
  {
    if (!find(key))
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