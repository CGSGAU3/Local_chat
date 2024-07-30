#include <iostream>
#include <fstream>

#include "json.h"

int main( void )
{
  Json j;

  j.parseFile("a.json");

  bool a = (bool)j["bruh"];

  std::ofstream f("out.json");
  std::string val = j.stringify();

  f.write(val.c_str(), val.length());
  std::cout << "Done!" << std::endl;
}