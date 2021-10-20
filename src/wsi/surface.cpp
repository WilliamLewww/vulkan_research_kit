#include "vrk/wsi/surface.h"

#ifdef X_LIB_SURFACE
#include <X11/Xlib.h>
typedef VkFlags VkXlibSurfaceCreateFlagsKHR;
struct VkXlibSurfaceCreateInfoKHR {
  VkStructureType sType;
  const void *pNext;
  VkXlibSurfaceCreateFlagsKHR flags;
  Display *dpy;
  Window window;
};
typedef VkResult(VKAPI_PTR *PFN_vkCreateXlibSurfaceKHR)(
    VkInstance instance, const VkXlibSurfaceCreateInfoKHR *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface);
typedef VkBool32(VKAPI_PTR *PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR)(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, Display *dpy,
    VisualID visualID);
#endif

VkBool32 Surface::checkPhysicalDeviceSurfaceSupport(
    VkPhysicalDevice &physicalDeviceHandleRef, uint32_t queueFamilyIndex,
    VkSurfaceKHR &surfaceHandleRef) {

  VkBool32 isPresentSupported = false;

  VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
      physicalDeviceHandleRef, queueFamilyIndex, surfaceHandleRef,
      &isPresentSupported);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkGetPhysicalDeviceSurfaceSupportKHR");
  }

  return isPresentSupported;
}

VkBool32 Surface::checkPhysicalDeviceXlibSurfaceSupport(
    VkInstance &instanceHandleRef, VkPhysicalDevice &physicalDeviceHandleRef,
    uint32_t queueFamilyIndex, void *displayPtr, void *visualIDPtr) {

  VkBool32 isPresentSupported = false;

#ifdef X_LIB_SURFACE
  LOAD_INSTANCE_FUNCTION(instanceHandleRef,
                         vkGetPhysicalDeviceXlibPresentationSupportKHR,
                         pvkGetPhysicalDeviceXlibPresentationSupportKHR);

  isPresentSupported = pvkGetPhysicalDeviceXlibPresentationSupportKHR(
      physicalDeviceHandleRef, queueFamilyIndex, (Display *)displayPtr,
      *((VisualID *)visualIDPtr));
#endif

  return isPresentSupported;
}

Surface::Surface(VkInstance &instanceHandleRef, Platform platform,
                 std::shared_ptr<void> surfaceCreateInfoParamPtr)
    : instanceHandleRef(instanceHandleRef) {

  this->surfaceHandle = VK_NULL_HANDLE;

  if (platform == Platform::Xlib) {
#ifdef X_LIB_SURFACE
    std::shared_ptr<XlibSurfaceCreateInfoParam> surfaceCreateInfoParam =
        std::static_pointer_cast<XlibSurfaceCreateInfoParam>(
            surfaceCreateInfoParamPtr);

    VkXlibSurfaceCreateInfoKHR xlibSurfaceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .dpy = (Display *)surfaceCreateInfoParam->displayPtr,
        .window = *(
            std::static_pointer_cast<Window>(surfaceCreateInfoParam->windowPtr)
                .get())};

    LOAD_INSTANCE_FUNCTION(instanceHandleRef, vkCreateXlibSurfaceKHR,
                           pvkCreateXlibSurfaceKHR);

    VkResult result = pvkCreateXlibSurfaceKHR(
        instanceHandleRef, &xlibSurfaceCreateInfo, NULL, &this->surfaceHandle);

    if (result != VK_SUCCESS) {
      throwExceptionVulkanAPI(result, "vkCreateXlibSurfaceKHR");
    }
#endif
  }
}

Surface::~Surface() {
  vkDestroySurfaceKHR(this->instanceHandleRef, this->surfaceHandle, NULL);
}

VkSurfaceCapabilitiesKHR Surface::getPhysicalDeviceSurfaceCapabilities(
    VkPhysicalDevice &physicalDeviceHandleRef) {

  VkSurfaceCapabilitiesKHR surfaceCapabilities;

  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      physicalDeviceHandleRef, this->surfaceHandle, &surfaceCapabilities);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result,
                            "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
  }

  return surfaceCapabilities;
}

std::vector<VkSurfaceFormatKHR> Surface::getPhysicalDeviceSurfaceFormatList(
    VkPhysicalDevice &physicalDeviceHandleRef) {

  uint32_t surfaceFormatCount = 0;

  VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(
      physicalDeviceHandleRef, this->surfaceHandle, &surfaceFormatCount, NULL);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkGetPhysicalDeviceSurfaceFormatsKHR");
  }

  std::vector<VkSurfaceFormatKHR> surfaceFormatList(surfaceFormatCount);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(
      physicalDeviceHandleRef, this->surfaceHandle, &surfaceFormatCount,
      surfaceFormatList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkGetPhysicalDeviceSurfaceFormatsKHR");
  }

  return surfaceFormatList;
}

std::vector<VkPresentModeKHR> Surface::getPhysicalDeviceSurfacePresentModeList(
    VkPhysicalDevice &physicalDeviceHandleRef) {

  uint32_t presentModeCount = 0;

  VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDeviceHandleRef, this->surfaceHandle, &presentModeCount, NULL);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result,
                            "vkGetPhysicalDeviceSurfacePresentModesKHR");
  }

  std::vector<VkPresentModeKHR> presentModeList(presentModeCount);
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDeviceHandleRef, this->surfaceHandle, &presentModeCount,
      presentModeList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result,
                            "vkGetPhysicalDeviceSurfacePresentModesKHR");
  }

  return presentModeList;
}

void Surface::queuePresentCmd(VkQueue &queueHandleRef,
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
      .pResults = resultPtr.get()};

  VkResult result = vkQueuePresentKHR(queueHandleRef, &presentInfo);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkQueuePresentKHR");
  }
}

VkSurfaceKHR &Surface::getSurfaceHandleRef() { return this->surfaceHandle; }
