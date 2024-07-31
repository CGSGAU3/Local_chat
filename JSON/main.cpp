#include <iostream>
#include <fstream>

#include "json.h"

int main( void )
{
  Json j;

  j.parseFile("a.json");


  j["bruh"] = "lol";

  Json::Array arr = j["library"]["books"];

  arr[0]["author"] = "Andrew Usikov";
  arr[0]["title"] = "How to C++";
  arr[0]["published"] = 2024;
  arr[0]["genres"][0] = "C++";
  arr[0]["genres"][1] = "Programming";

  j["library"]["books"] = arr;

  j.saveToFile("out2.json");
  std::cout << "Done!" << std::endl;
}