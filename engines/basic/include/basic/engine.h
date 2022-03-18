#pragma once
#include "basic/camera.h"
#include "basic/scene.h"

#include <vrk/command_buffer_group.h>
#include <vrk/command_pool.h>
#include <vrk/device.h>
#include <vrk/image.h>
#include <vrk/image_view.h>
#include <vrk/instance.h>

#include <vrk/wsi/surface.h>
#include <vrk/wsi/swapchain.h>

#include <vrk/ray_tracing/ray_tracing.h>

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

  std::shared_ptr<Device> getDevicePtr();

  std::shared_ptr<VkPhysicalDevice> getPhysicalDeviceHandlePtr();

  uint32_t getQueueFamilyIndex();

  std::shared_ptr<CommandBufferGroup> getCommandBufferGroupPtr();

  uint32_t getSecondaryCommandBufferCount();

  std::shared_ptr<CommandBufferGroup> getSecondaryCommandBufferGroupPtr();

  uint32_t getUtilityCommandBufferCount();

  std::shared_ptr<CommandBufferGroup> getUtilityCommandBufferGroupPtr();

  VkSurfaceCapabilitiesKHR getSurfaceCapabilities();

  std::vector<VkSurfaceFormatKHR> getSurfaceFormatList();

  std::vector<std::shared_ptr<Image>> getSwapchainImagePtrList();

  uint32_t getSwapchainImageCount();

private:
  bool isValidationEnabled;

  bool isRayTracingEnabled;

  std::unique_ptr<Instance> instancePtr;

  std::vector<VkPhysicalDevice> physicalDeviceHandleList;

  std::unique_ptr<Surface> surfacePtr;

  std::shared_ptr<VkPhysicalDevice> physicalDeviceHandlePtr;

  uint32_t queueFamilyIndex;

  std::shared_ptr<Device> devicePtr;

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList;

  std::unique_ptr<CommandPool> commandPoolPtr;

  std::shared_ptr<CommandBufferGroup> commandBufferGroupPtr;

  uint32_t secondaryCommandBufferCount;

  std::shared_ptr<CommandBufferGroup> secondaryCommandBufferGroupPtr;

  uint32_t utilityCommandBufferCount;

  std::shared_ptr<CommandBufferGroup> utilityCommandBufferGroupPtr;

  VkSurfaceCapabilitiesKHR surfaceCapabilities;

  std::vector<VkSurfaceFormatKHR> surfaceFormatList;

  std::vector<VkPresentModeKHR> presentModeList;

  std::unique_ptr<Swapchain> swapchainPtr;

  std::vector<std::shared_ptr<Image>> swapchainImagePtrList;

  std::vector<std::unique_ptr<ImageView>> swapchainImageViewPtrList;

  std::vector<std::unique_ptr<Fence>> imageAvailableFencePtrList;

  std::vector<std::unique_ptr<Semaphore>> acquireImageSemaphorePtrList;

  std::vector<std::unique_ptr<Semaphore>> writeImageSemaphorePtrList;

  std::vector<std::shared_ptr<Scene>> scenePtrList;

  std::vector<std::shared_ptr<Camera>> cameraPtrList;

  uint32_t currentFrame;
};
