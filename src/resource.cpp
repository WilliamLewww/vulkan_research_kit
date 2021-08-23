#include "vrk/resource.h"

std::string Resource::findResource(std::string resourcePath) {
  std::ifstream f = std::ifstream(resourcePath.c_str());
  if (f.good()) {
    return resourcePath.c_str();
  }
  else if (std::getenv("VRK_RESOURCE_PATH") != NULL) {
    std::string envResourcePath = std::getenv("VRK_RESOURCE_PATH");

    f = std::ifstream(envResourcePath + resourcePath);
    if (f.good()) {
      return envResourcePath + resourcePath;
    }
  }

  return "";
}