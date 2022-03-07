#include "vrk/resource.h"

std::string Resource::findResource(std::string resourcePath) {
  std::ifstream f = std::ifstream(resourcePath.c_str());
  if (f.good()) {
    return resourcePath.c_str();
  } else if (std::getenv("VRK_RESOURCES_DIR") != NULL) {
    std::string envResourcePath = std::getenv("VRK_RESOURCES_DIR");

    f = std::ifstream(envResourcePath + resourcePath);
    if (f.good()) {
      return envResourcePath + resourcePath;
    }
  } else {
#ifdef VRK_RESOURCES_DIR
    f = std::ifstream(VRK_RESOURCES_DIR + resourcePath);
    if (f.good()) {
      return VRK_RESOURCES_DIR + resourcePath;
    }
#endif
  }

  return "";
}
