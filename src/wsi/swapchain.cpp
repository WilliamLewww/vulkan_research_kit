#include "vrk/wsi/swapchain.h"

Swapchain::Swapchain(VkDevice& deviceHandleRef,
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
    VkSwapchainKHR oldSwapchainHandle) :
    deviceHandleRef(deviceHandleRef) {

  this->swapchainHandle = VK_NULL_HANDLE;

  VkSwapchainCreateInfoKHR swapchainCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = NULL,
    .flags = swapchainCreateFlags,
    .surface = surfaceHandleRef,
    .minImageCount = minImageCount,
    .imageFormat = imageFormat,
    .imageColorSpace = imageColorSpace,
    .imageExtent = imageExtent2D,
    .imageArrayLayers = imageArrayLayers,
    .imageUsage = imageUsageFlags,
    .imageSharingMode = imageSharingMode,
    .queueFamilyIndexCount = (uint32_t)queueFamilyIndexList.size(),
    .pQueueFamilyIndices = queueFamilyIndexList.data(),
    .preTransform = surfaceTransformFlagBits,
    .compositeAlpha = compositeAlphaFlagBits,
    .presentMode = presentMode,
    .clipped = clipped,
    .oldSwapchain = oldSwapchainHandle
  };

  VkResult result = vkCreateSwapchainKHR(deviceHandleRef, &swapchainCreateInfo,
      NULL, &this->swapchainHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateSwapchainKHR");
  }
}

Swapchain::~Swapchain() {
  vkDestroySwapchainKHR(this->deviceHandleRef, this->swapchainHandle, NULL);
}

VkResult Swapchain::getSwapchainStatus(VkInstance& instanceHandleRef) {
  LOAD_INSTANCE_FUNCTION(instanceHandleRef, vkGetSwapchainStatusKHR,
      pvkGetSwapchainStatusKHR);

  VkResult result = pvkGetSwapchainStatusKHR(this->deviceHandleRef,
      this->swapchainHandle);

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throwExceptionVulkanAPI(result, "vkGetSwapchainStatusKHR");
  }

  return result;
}

uint32_t Swapchain::aquireNextImageIndex(uint64_t timeout,
    VkSemaphore semaphoreHandle,
    VkFence fenceHandle) {

  uint32_t imageIndex = 0;

  VkResult result = vkAcquireNextImageKHR(this->deviceHandleRef,
      this->swapchainHandle,
      timeout,
      semaphoreHandle,
      fenceHandle,
      &imageIndex);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkAcquireNextImageKHR");
  }

  return imageIndex;
}

void Swapchain::queuePresentCmd(VkQueue& queueHandleRef,
    std::vector<VkSemaphore> waitSemaphoreHandleList,
    std::vector<VkSwapchainKHR> swapchainHandleList,
    std::vector<uint32_t> imageIndexList,
    std::shared_ptr<VkResult[]> resultPtr) {

  VkPresentInfoKHR presentInfo = {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = NULL,
    .waitSemaphoreCount = (uint32_t)waitSemaphoreHandleList.size(),
    .pWaitSemaphores = waitSemaphoreHandleList.data(),
    .swapchainCount = (uint32_t)swapchainHandleList.size(),
    .pSwapchains = swapchainHandleList.data(),
    .pImageIndices = imageIndexList.data(),
    .pResults = resultPtr.get()
  };

  VkResult result = vkQueuePresentKHR(queueHandleRef, &presentInfo);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkQueuePresentKHR");
  }
}

std::vector<VkImage> Swapchain::getSwapchainImageList() {
  uint32_t imageCount = 0;

  VkResult result = vkGetSwapchainImagesKHR(this->deviceHandleRef,
      this->swapchainHandle, &imageCount, NULL);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkGetSwapchainImagesKHR");
  }

  std::vector<VkImage> imageList(imageCount);
  result = vkGetSwapchainImagesKHR(this->deviceHandleRef,
      this->swapchainHandle, &imageCount, imageList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkGetSwapchainImagesKHR");
  }

  return imageList;
}