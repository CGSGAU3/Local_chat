#include <fstream>

#include "json.h"

void JsonArray::parse( std::istream &istr )
{
  char ch = 0;

  while (true)
  {
    std::string newArg;
    Json newObj;
    JsonArray newArr;

    if (ch == ']')
      break;

    SkipSpaces(istr, ch);

    if (ch == ']')
      break;

    auto checkZpt = [&]( void )
    {
      //if (ch != ',' && isspace((unsigned char)ch))
      SkipSpaces(istr, ch);
      if (ch != ',' && ch != ']')
        throw "Invalid JSON array!";
    };

    switch (ch)
    {
    case '"':
      ReadWhileNot(istr, ch, '"', if (ch != '"') newArg += ch);
      arr.push_back(newArg);
      checkZpt();
      break;
    case '{':
      newObj.parse(istr, true);
      arr.push_back(newObj);
      checkZpt();
      break;
    case '[':
      newArr.parse(istr);
      arr.push_back(newArr);
      checkZpt();
      break;
    case ',':
      continue;
    default:
      newArg += ch;
      do
      {
        istr.read(&ch, 1);
        if (ch != ']' && ch != ',')
          newArg += ch;
      } while (istr && ch != ',' && ch != ']');

      if (!istr)
        throw "Invalid JSON array!";

      char *end;
      double numVal = strtod(newArg.c_str(), &end);

      if (*end == 0)
        arr.push_back(numVal);
      else
      {
        if (newArg == "null")
          arr.push_back(nullptr);
        else if (newArg == "true")
          arr.push_back(true);
        else if (newArg == "false")
          arr.push_back(false);
        else
          throw "Invalid JSON array!";
      }

      if (ch == ']')
        return;
      break;
    }
  }
}

std::string JsonArray::stringify( void ) const
{
  return "";
}

template<typename T>
bool JsonArray::at( const size_t index, T &data )
{
  if (index >= arr.size())
    return false;

  try
  {
    data = std::get<T>(arr[index]);
    return true;
  }
  catch (...)
  {
    return false;
  }
}

void Json::parse( std::istream &istr, bool isRec )
{
  char ch;
  std::string key;

  istr.read(&ch, 1);
  if (ch == '}')
    return;
  if (!isRec)
    if (ch != '{')
      throw "Invalid JSON!";
  while (true)
  {
    key = "";
    if (ch != '"')
      SkipSpaces(istr, ch);
    if (ch == '"')
    {
      ReadWhileNot(istr, ch, '"', if (ch != '"') key += ch);
      SkipSpaces(istr, ch);
      if (ch != ':')
        throw "Invalid JSON!";
      SkipSpaces(istr, ch);

      std::string value;
      Json recObj;
      JsonArray arr;

      switch (ch)
      {
      case '"':
        ReadWhileNot(istr, ch, '"', if (ch != '"') value += ch);
        obj.insert({key, value});
        SkipSpaces(istr, ch);
        if (ch == ',')
          continue;
        else if (ch == '}')
          return;
        else
          throw "Invalid JSON!";
        break;
      case '{':
        recObj.parse(istr, true);
        obj.insert({key, recObj});
        SkipSpaces(istr, ch);
        if (ch == ',')
          continue;
        else if (ch == '}')
          return;
        else
          throw "Invalid JSON!";
        break;
      case '[':
        arr.parse(istr);
        obj.insert({key, arr});
        SkipSpaces(istr, ch);
        if (ch == ',')
          continue;
        else if (ch == '}')
          return;
        else 
          throw "Invalid JSON!";
      default:
        // TODO: same shit as array
        value += ch;
        do
        {
          istr.read(&ch, 1);
          if (ch != '}' && ch != ',' && !isspace((unsigned char)ch))
            value += ch;
        } while (ch != ',' && ch != '}' && !isspace((unsigned char)ch));
        obj.insert({key, value});
        if (ch == '}')
          return;
        continue;
      }
    }
    else if (ch == '}')
      return;
    else
      throw "Invalid JSON!";
  }
}

void Json::parse( const std::string &str )
{
  std::istringstream istr(str);
  std::fstream f(str);

  parse(f);
}

std::string Json::stringify( void ) const
{
  return "";
}

FieldType & Json::operator[]( const std::string &key )
{
  return obj[key];
}

template<typename T>
bool Json::at( const std::string &key, T &data )
{
  if (obj.find(key) == obj.end())
    return false;

  try
  {
    data = std::get<T>(obj[key]);
    return true;
  }
  catch (...)
  {
    return false;
  }
}