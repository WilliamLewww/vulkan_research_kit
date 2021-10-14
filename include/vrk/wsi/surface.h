#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

class Surface {
public:
  enum class Platform {
    Android,
    Wayland,
    Win32,
    XCB,
    Xlib,
    DirectFB,
    Fuchsia,
    GoogleGames,
    iOS,
    macOS,
    VI,
    Metal,
    QNX
  };

  struct XlibSurfaceCreateInfoParam {
    void* displayPtr;
    std::shared_ptr<void> windowPtr;
  };

  static VkBool32 checkPhysicalDeviceSurfaceSupport(
      VkPhysicalDevice& physicalDeviceHandleRef,
      uint32_t queueFamilyIndex,
      VkSurfaceKHR& surfaceHandleRef);

  static VkBool32 checkPhysicalDeviceXlibSurfaceSupport(
      VkInstance& instanceHandleRef,
      VkPhysicalDevice& physicalDeviceHandleRef,
      uint32_t queueFamilyIndex,
      void* displayPtr,
      void* visualIDPtr);

  Surface(VkInstance& instanceHandleRef,
      Platform platform,
      std::shared_ptr<void> surfaceCreateInfoParamPtr);

  ~Surface();

  VkSurfaceCapabilitiesKHR getPhysicalDeviceSurfaceCapabilities(
      VkPhysicalDevice& physicalDeviceHandleRef);

  std::vector<VkSurfaceFormatKHR> getPhysicalDeviceSurfaceFormatList(
      VkPhysicalDevice& physicalDeviceHandleRef);

  std::vector<VkPresentModeKHR> getPhysicalDeviceSurfacePresentModeList(
      VkPhysicalDevice& physicalDeviceHandleRef);

  void queuePresentCmd(VkQueue& queueHandleRef,
      std::vector<VkSemaphore> waitSemaphoreHandleList,
      std::vector<VkSwapchainKHR> swapchainHandleList,
      std::vector<uint32_t> imageIndexList,
      std::shared_ptr<VkResult[]> resultPtr);

  VkSurfaceKHR& getSurfaceHandleRef();
private:
  VkSurfaceKHR surfaceHandle;

  VkInstance& instanceHandleRef;
};