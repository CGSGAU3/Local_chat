#include <iostream>
#include <fstream>

#include "json.h"

struct foo
{
  JSON_FIELDS(foo, (int) a, (double) b, (std::vector<int>) c);
};


int main( void )
{
  Json test;

  /*test["a"] = 30;
  test["b"] = 47;
  test["c"] = Json::Array(5);

  for (int i = 0; i < 5; i++)
    test["c"][i] = pow(2, i);*/
  //test.toObject(x);

  foo x;

  x.a = 3047;
  x.b = 102;
  x.c = {30, 47, 102, 69, 96};
  test = x;

  test.saveToFile("test_serializer.json");

  Json j;

  j.parseFile("ttt.json");

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
