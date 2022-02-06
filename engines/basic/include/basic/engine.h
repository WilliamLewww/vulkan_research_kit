#pragma once
#include "basic/camera.h"
#include "basic/scene.h"

#include <vrk/command_buffer_group.h>
#include <vrk/command_pool.h>
#include <vrk/device.h>
#include <vrk/framebuffer.h>
#include <vrk/image.h>
#include <vrk/image_view.h>
#include <vrk/instance.h>
#include <vrk/render_pass.h>

#include <vrk/wsi/surface.h>
#include <vrk/wsi/swapchain.h>

#include "vrk/ray_tracing/ray_tracing.h"

#include <X11/Xlib.h>

#include <map>

class Engine : public std::enable_shared_from_this<Engine> {
public:
  Engine(std::string appName, bool isValidationEnabled,
         bool isRayTracingEnabled);

  ~Engine();

  std::vector<std::string> getPhysicalDeviceNameList();

  void selectWindow(Display *displayPtr, std::shared_ptr<Window> windowPtr);

  void selectPhysicalDevice(std::string physicalDeviceName);

  std::shared_ptr<Scene> createScene(std::string sceneName);

  std::shared_ptr<Camera> createCamera(std::string cameraName);

  uint32_t render(std::shared_ptr<Scene> scenePtr,
                  std::shared_ptr<Camera> cameraPtr);

private:
  bool isValidationEnabled;

  bool isRayTracingEnabled;

  std::unique_ptr<Instance> instancePtr;

  std::vector<VkPhysicalDevice> physicalDeviceHandleList;

  std::unique_ptr<Surface> surfacePtr;

  std::unique_ptr<VkPhysicalDevice> physicalDeviceHandlePtr;

  uint32_t queueFamilyIndex;

  std::shared_ptr<Device> devicePtr;

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList;

  std::unique_ptr<CommandPool> commandPoolPtr;

  std::unique_ptr<CommandBufferGroup> commandBufferGroupPtr;

  uint32_t secondaryCommandBufferCount;

  std::unique_ptr<CommandBufferGroup> secondaryCommandBufferGroupPtr;

  uint32_t utilityCommandBufferCount;

  std::unique_ptr<CommandBufferGroup> utilityCommandBufferGroupPtr;

  VkSurfaceCapabilitiesKHR surfaceCapabilities;

  std::vector<VkSurfaceFormatKHR> surfaceFormatList;

  std::vector<VkPresentModeKHR> presentModeList;

  std::unique_ptr<Swapchain> swapchainPtr;

  std::unique_ptr<RenderPass> renderPassPtr;

  std::vector<VkImage> swapchainImageHandleList;

  std::vector<std::unique_ptr<ImageView>> swapchainImageViewPtrList;

  std::vector<std::unique_ptr<Image>> depthImagePtrList;

  std::vector<std::unique_ptr<ImageView>> depthImageViewPtrList;

  std::vector<std::unique_ptr<Framebuffer>> framebufferPtrList;

  std::vector<std::unique_ptr<Fence>> imageAvailableFencePtrList;

  std::vector<std::unique_ptr<Semaphore>> acquireImageSemaphorePtrList;

  std::vector<std::unique_ptr<Semaphore>> writeImageSemaphorePtrList;

  std::vector<std::shared_ptr<Scene>> scenePtrList;

  std::vector<std::shared_ptr<Camera>> cameraPtrList;

  uint32_t currentFrame;

  friend class Scene;
  friend class Camera;
  friend class Material;
  friend class MaterialRaster;
  friend class Model;
  friend class Light;
};
