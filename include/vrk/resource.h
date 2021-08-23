#pragma once

#include <string>
#include <fstream>
#include <cstdlib>

class Resource {
public:
  static std::string findResource(std::string resourcePath);
};