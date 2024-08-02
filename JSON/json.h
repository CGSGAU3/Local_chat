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

#define GENERATE_OPTION_SET_BASE(optType, idx, objType)                \
  if (obj[fi.name].index() == idx &&                                   \
      fi.type == typeid(optType).name())                               \
  {                                                                    \
    setData(ptr, fi.offset, (optType)std::get<objType>(obj[fi.name])); \
    continue;                                                          \
  }

#define GENERATE_OPTION_ARRAY_SET(optType)                                      \
  if (obj[fi.name].index() == 5 &&                                              \
      fi.type == typeid(std::vector<optType>).name())                           \
  {                                                                             \
    setData(ptr, fi.offset, std::get<Array>(obj[fi.name]).toVector<optType>()); \
    continue;                                                                   \
  }


#define GENERATE_OPTION_SET(optTypeBase, idx, objType)          \
  GENERATE_OPTION_SET_BASE(optTypeBase, idx, objType);          \
  GENERATE_OPTION_SET_BASE(signed optTypeBase, idx, objType);   \
  GENERATE_OPTION_SET_BASE(unsigned optTypeBase, idx, objType); \
  GENERATE_OPTION_ARRAY_SET(optTypeBase);                       \
  GENERATE_OPTION_ARRAY_SET(signed optTypeBase);                \
  GENERATE_OPTION_ARRAY_SET(unsigned optTypeBase);

#define GENERATE_OPTION_GET_BASE(optType, idx, objType)       \
  if (fi.type == typeid(optType).name())                      \
  {                                                           \
    obj[fi.name] = (objType)getData<optType>(ptr, fi.offset); \
    continue;                                                 \
  }

#define GENERATE_OPTION_ARRAY_GET(optType)                                           \
  if (fi.type == typeid(std::vector<optType>).name())                                \
  {                                                                                  \
    obj[fi.name] = Array::fromVector(getData<std::vector<optType>>(ptr, fi.offset)); \
    continue;                                                                        \
  }

#define GENERATE_OPTION_GET(optTypeBase, idx, objType)          \
  GENERATE_OPTION_GET_BASE(optTypeBase, idx, objType);          \
  GENERATE_OPTION_GET_BASE(signed optTypeBase, idx, objType);   \
  GENERATE_OPTION_GET_BASE(unsigned optTypeBase, idx, objType); \
  GENERATE_OPTION_ARRAY_GET(optTypeBase);                       \
  GENERATE_OPTION_ARRAY_GET(signed optTypeBase);                \
  GENERATE_OPTION_ARRAY_GET(unsigned optTypeBase);

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

    template<typename T>
    static Array fromVector( const std::vector<T> &v )
    {
      Array res;

      for (const auto &el : v)
        res.add(el);
      return res;
    }

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
    std::vector<T> toVector( void )
    {
      std::vector<T> res;

      for (size_t i = 0; i < arr.size(); i++)
      {
        if constexpr (std::is_arithmetic_v<T>)
          res.push_back((T)std::get<double>(arr[i]));
        else
          res.push_back(std::get<T>(arr[i]));
      }

      return res;
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

  template<typename T>
  Json( const T &any ) : obj()
  {
    fromObject(any);
  }

  template<typename T>
  Json & operator =( const T &any )
  {
    fromObject(any);
    return *this;
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

private:

  template<typename T>
  static T & getData( char *ptr, int offset )
  {
    return *(T *)(ptr + offset);
  }

  template<typename T>
  static void setData( char *ptr, int offset, const T &data )
  {
    *(T *)(ptr + offset) = data;
  }

public:

  template<typename T>
  void fromObject( const T &data )
  {
    TypeInfo ti = T::getType();
    char *ptr = (char *)&data;

    for (const auto &fi : ti.fields)
    {
      GENERATE_OPTION_GET_BASE(bool, 0, bool);
      GENERATE_OPTION_GET(short, 1, double);
      GENERATE_OPTION_GET(int, 1, double);
      GENERATE_OPTION_GET(long, 1, double);
      GENERATE_OPTION_GET(long long, 1, double);
      GENERATE_OPTION_GET_BASE(float, 1, double);
      GENERATE_OPTION_GET_BASE(double, 1, double);
      GENERATE_OPTION_GET_BASE(long double, 1, double);
      GENERATE_OPTION_ARRAY_GET(float);
      GENERATE_OPTION_ARRAY_GET(double);
      GENERATE_OPTION_ARRAY_GET(long double);
      GENERATE_OPTION_GET_BASE(std::string, 2, std::string);

      if (fi.type == "signed char" || fi.type == "unsigned char" || fi.type == "char")
      {
        std::string str = " ";

        str[0] = getData<char>(ptr, fi.offset);
        obj[fi.name] = str;
        continue;
      }

      throw std::exception("Unknown type!");
    }
  }

  template<typename T>
  void toObject( T &data )
  {
    TypeInfo ti = T::getType();
    char *ptr = (char *)&data;

    for (const auto &fi : ti.fields)
    {
      if (obj.find(fi.name) == obj.end())
        throw std::exception("Unknown field!");

      GENERATE_OPTION_SET_BASE(bool, 0, bool);
      GENERATE_OPTION_SET(char, 1, double);
      GENERATE_OPTION_SET(short, 1, double);
      GENERATE_OPTION_SET(int, 1, double);
      GENERATE_OPTION_SET(long, 1, double);
      GENERATE_OPTION_SET(long long, 1, double);
      GENERATE_OPTION_SET_BASE(float, 1, double);
      GENERATE_OPTION_SET_BASE(double, 1, double);
      GENERATE_OPTION_SET_BASE(long double, 1, double);
      GENERATE_OPTION_ARRAY_SET(float);
      GENERATE_OPTION_ARRAY_SET(double);
      GENERATE_OPTION_ARRAY_SET(long double);
      GENERATE_OPTION_SET_BASE(std::string, 2, std::string);

      if (obj[fi.name].index() == 2 && (fi.type == "signed char" || 
          fi.type == "unsigned char" || fi.type == "char"))
      {
        const std::string &str = std::get<std::string>(obj[fi.name]);

        if (str.length() == 1)
          setData(ptr, fi.offset, str[0]);
        else
          throw std::exception("Attempt to equal string and single char!");
        continue;
      }

      throw std::exception("Unknown type!");
    }
  }
};