#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

class Swapchain {
public:
  Swapchain(VkDevice& deviceHandleRef,
      VkSwapchainCreateFlagsKHR swapchainCreateFlags,
      VkSurfaceKHR& surfaceHandleRef,
      uint32_t minImageCount,
      VkFormat imageFormat,
      VkColorSpaceKHR imageColorSpace,
      VkExtent2D imageExtent2D,
      uint32_t imageArrayLayers,
      VkImageUsageFlags imageUsageFlags,
      VkSharingMode imageSharingMode,
      std::vector<uint32_t> queueFamilyIndexList,
      VkSurfaceTransformFlagBitsKHR surfaceTransformFlagBits,
      VkCompositeAlphaFlagBitsKHR compositeAlphaFlagBits,
      VkPresentModeKHR presentMode,
      VkBool32 clipped,
      VkSwapchainKHR oldSwapchainHandle);

  ~Swapchain();

  VkResult getSwapchainStatus(VkInstance& instanceHandleRef);

  uint32_t aquireNextImageIndex(uint64_t timeout,
      VkSemaphore semaphoreHandle,
      VkFence fenceHandle);

  void queuePresentCmd(VkQueue& queueHandleRef,
      std::vector<VkSemaphore> waitSemaphoreHandleList,
      std::vector<VkSwapchainKHR> swapchainHandleList,
      std::vector<uint32_t> imageIndexList,
      std::shared_ptr<VkResult[]> resultPtr);

  std::vector<VkImage> getSwapchainImageList();
private:
  VkSwapchainKHR swapchainHandle;

  VkDevice& deviceHandleRef;
};