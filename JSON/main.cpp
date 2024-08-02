#include <iostream>
#include <fstream>

#include "json.h"

struct foo
{
  JSON_FIELDS(foo, (int) a, (double) b);
};


int main( void )
{
  int a = 10;

  foo x;

  auto ti = x.getType();
  auto ttt = typeid(float).hash_code();

  char *sss = (char *)&x;
  double *dbl = (double *)(sss + ti.fields[1].offset);

  *dbl = 4;

  Json j;

  j.parseFile("a.json");
  j["CGSG"] = "AU3";

  Json::Array &arr = j["library"]["books"];

  arr[0]["author"] = "Andrew Usikov";
  arr[0]["title"] = "How to C++";
  arr[0]["published"] = 2024;
  arr[0]["genres"][0] = "C++";
  arr[0]["genres"][1] = "Programming";
  arr[0]["genres"][2] = "Lifestyle";

  j["library"]["addon"] = j["library"]["books"][0]["genres"][0];

  j.saveToFile("out2.json");
  std::cout << "Done!" << std::endl;
}
