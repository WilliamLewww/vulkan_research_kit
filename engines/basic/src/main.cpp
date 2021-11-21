#include "basic/engine.h"

int main() {
  Display *displayPtr = XOpenDisplay(NULL);
  int screen = DefaultScreen(displayPtr);

  Window window = XCreateSimpleWindow(
      displayPtr, RootWindow(displayPtr, screen), 10, 10, 100, 100, 1,
      BlackPixel(displayPtr, screen), WhitePixel(displayPtr, screen));

  XSelectInput(displayPtr, window, ExposureMask | KeyPressMask);
  XMapWindow(displayPtr, window);

  Engine *engine = new Engine("Demo Application", true, {}, {});

  engine->selectWindow(displayPtr, std::make_shared<Window>(window));

  std::vector<std::string> physicalDeviceNameList =
      engine->getPhysicalDeviceNameList();

  for (std::string physicalDeviceName : physicalDeviceNameList) {
    printf("%s\n", physicalDeviceName.c_str());
  }

  engine->selectPhysicalDevice(physicalDeviceNameList[0], {});

  std::shared_ptr<Material> material = engine->createMaterial(
      "my-material", "resources/shaders/material.vert.spv",
      "resources/shaders/material.frag.spv");

  std::shared_ptr<Model> model =
      engine->createModel("my-model", "resources/models/model.obj", material);

  return 0;
}
