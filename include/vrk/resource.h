#pragma once

#include <cstdlib>
#include <fstream>
#include <string>

class Resource {
public:
  static std::string findResource(std::string resourcePath);
};