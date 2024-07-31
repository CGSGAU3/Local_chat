#include <fstream>

#include "json.h"

void Json::Array::parse( std::istream &istr )
{
  char ch = 0;

  while (true)
  {
    std::string newArg;
    Json newObj;
    Array newArr;

    if (ch == ']')
      break;

    SkipSpaces(istr, ch);

    if (ch == ']')
      break;

    auto checkZpt = [&]( void )
    {
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
        if (ch != ']' && ch != ',' && !isspace((unsigned char)ch))
          newArg += ch;
      } while (istr && ch != ',' && ch != ']' && !isspace((unsigned char)ch));

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

void Json::Array::save( std::ostream &ostr, int prec ) const
{
  ostr.write("[\r\n", 3);

  for (size_t i = 0; i < arr.size(); i++)
  {
    for (int i = 0; i < prec + 4; i++)
      ostr.write(" ", 1);
    if (arr[i].index() == 4)
      std::get<Json>(arr[i]).save(ostr, prec + 4);
    else if (arr[i].index() == 5)
      std::get<Array>(arr[i]).save(ostr, prec + 4);
    else
    {
      std::string val = Json::to_string(arr[i]);

      ostr.write(val.c_str(), val.length());
    }

    if (i != arr.size() - 1)
      ostr.write(",", 1);
    ostr.write("\r\n", 2);
  }

  for (int i = 0; i < prec; i++)
    ostr.write(" ", 1);
  ostr.write("]", 1);
}

void Json::Array::saveRaw( std::ostream &ostr ) const
{
  ostr.write("[", 1);

  for (size_t i = 0; i < arr.size(); i++)
  {
    std::string val = Json::to_string(arr[i]);

    ostr.write(val.c_str(), val.length());
    if (i != arr.size() - 1)
      ostr.write(",", 1);
  }

  ostr.write("]", 1);
}

std::string Json::Array::stringify( void ) const
{
  std::ostringstream ostr;

  saveRaw(ostr);
  return ostr.str();
}

void Json::parse( std::istream &istr, bool isRec )
{
  char ch;
  std::string key;

  if (!istr)
    throw "Unexpected end of data!";

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
      Array arr;

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
        value += ch;
        do
        {
          istr.read(&ch, 1);
          if (ch != '}' && ch != ',' && !isspace((unsigned char)ch))
            value += ch;
        } while (istr && ch != ',' && ch != '}' && !isspace((unsigned char)ch));

        if (!istr)
          throw "Invalid JSON!";

        char *end;
        double numVal = strtod(value.c_str(), &end);

        if (*end == 0)
          obj.insert({key, numVal});
        else
        {
          if (value == "null")
            obj.insert({key, nullptr});
          else if (value == "true")
            obj.insert({key, true});
          else if (value == "false")
            obj.insert({key, false});
          else
            throw "Invalid JSON array!";
        }

        if (ch == ']')
          return;
        break;
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

  parse(istr);
}

void Json::parseFile( const std::string &filename )
{
  std::ifstream file(filename);

  parse(file);
}

void Json::save( std::ostream &ostr, int prec ) const
{
  ostr.write("{\r\n", 3);

  size_t count = 0;
  for (const auto &kv : obj)
  {
    std::string key = "";

    for (int i = 0; i < prec + 4; i++)
      key += ' ';
    key += "\"" + kv.first + "\": ";
    ostr.write(key.c_str(), key.length());

    if (kv.second.index() == 4)
      std::get<Json>(kv.second).save(ostr, prec + 4);
    else if (kv.second.index() == 5)
      std::get<Array>(kv.second).save(ostr, prec + 4);
    else
    {
      std::string val = to_string(kv.second);

      ostr.write(val.c_str(), val.length());
    }

    if (count++ != obj.size() - 1)
      ostr.write(",", 1);
    ostr.write("\r\n", 2);
  }

  for (int i = 0; i < prec; i++)
    ostr.write(" ", 1);
  ostr.write("}", 1);
}

void Json::saveToFile( const std::string &filename )
{
  std::ofstream file(filename);

  save(file);
}

void Json::saveRaw( std::ostream &ostr ) const
{
  ostr.write("{", 1);

  size_t count = 0;
  for (const auto &kv : obj)
  {
    std::string key = "\"" + kv.first + "\":";
    std::string val = to_string(kv.second);

    ostr.write(key.c_str(), key.length());
    ostr.write(val.c_str(), val.length());

    if (count++ != obj.size() - 1)
      ostr.write(",", 1);
  }

  ostr.write("}", 1);
}

std::string Json::stringify( void ) const
{
  std::ostringstream ostr;

  saveRaw(ostr);
  return ostr.str();
}