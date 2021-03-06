#include "basic/engine.h"

int main() {
  Display *displayPtr = XOpenDisplay(NULL);
  int screen = DefaultScreen(displayPtr);

  Window window = XCreateSimpleWindow(
      displayPtr, RootWindow(displayPtr, screen), 10, 10, 100, 100, 1,
      BlackPixel(displayPtr, screen), WhitePixel(displayPtr, screen));

  XSelectInput(displayPtr, window, ExposureMask | KeyPressMask);
  XMapWindow(displayPtr, window);

  std::shared_ptr<Engine> engine =
      std::shared_ptr<Engine>(new Engine("Demo Application", true, {}, {}));

  engine->selectWindow(displayPtr, std::make_shared<Window>(window));

  std::vector<std::string> physicalDeviceNameList =
      engine->getPhysicalDeviceNameList();

  for (std::string physicalDeviceName : physicalDeviceNameList) {
    printf("%s\n", physicalDeviceName.c_str());
  }

  engine->selectPhysicalDevice(physicalDeviceNameList[0], {});

  std::shared_ptr<Scene> scene = engine->createScene("my-scene");

  std::shared_ptr<Material> material1 = scene->createMaterial(
      "material1",
      {{Material::ShaderStage::VERTEX, "resources/shaders/voxels.vert.spv"},
       {Material::ShaderStage::GEOMETRY, "resources/shaders/voxels.geom.spv"},
       {Material::ShaderStage::FRAGMENT, "resources/shaders/voxels.frag.spv"}});

  std::shared_ptr<Material> material2 = scene->createMaterial(
      "material2",
      {{Material::ShaderStage::VERTEX, "resources/shaders/material.vert.spv"},
       {Material::ShaderStage::FRAGMENT,
        "resources/shaders/material.frag.spv"}});

  std::shared_ptr<Model> model1 = scene->createModel(
      "model1", "resources/models/utah_teapot/utah_teapot.obj", material1);

  std::shared_ptr<Model> model2 = scene->createModel(
      "model2", "resources/models/color_cube/color_cube.obj", material2);
  model2->setPosition(0, 2, 0);

  std::shared_ptr<Light> light =
      scene->createLight("my-light", Light::LIGHT_TYPE_POINT);

  std::shared_ptr<Camera> camera = engine->createCamera("my-camera");

  XEvent event;
  while (true) {
    XNextEvent(displayPtr, &event);
    if (event.type == KeyPress) {
      // escape
      if (event.xkey.keycode == 9) {
        break;
      }
      // left
      if (event.xkey.keycode == 113) {
        camera->updateRotation(-0.05, 0.0, 0.0);
      }
      // down
      if (event.xkey.keycode == 116) {
        camera->updatePosition(0.0, 0.0, -0.05);
      }
      // up
      if (event.xkey.keycode == 111) {
        camera->updatePosition(0.0, 0.0, 0.05);
      }
      // right
      if (event.xkey.keycode == 114) {
        camera->updateRotation(0.05, 0.0, 0.0);
      }
    }

    engine->render(scene, camera);
  }

  return 0;
}
