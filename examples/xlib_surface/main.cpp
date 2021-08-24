#include "vrk/instance.h"
#include "vrk/wsi/surface.h"
#include "vrk/device.h"
#include "vrk/command_pool.h"
#include "vrk/command_buffer_group.h"
#include "vrk/wsi/swapchain.h"
#include "vrk/render_pass.h"

#include <X11/Xlib.h>

#include <fstream>
#include <cstring>

int main(void) {
  Display* displayPtr = XOpenDisplay(NULL);
  int screen = DefaultScreen(displayPtr);

  Window window = XCreateSimpleWindow(displayPtr, RootWindow(displayPtr, screen), 10, 10, 100, 100, 1,
      BlackPixel(displayPtr, screen), WhitePixel(displayPtr, screen));

  XSelectInput(displayPtr, window, ExposureMask | KeyPressMask);
  XMapWindow(displayPtr, window);

  std::vector<VkValidationFeatureEnableEXT> validationFeatureEnableList = {
    VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
    VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
    VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
  };

  std::vector<VkValidationFeatureDisableEXT> validationFeatureDisableList = {};

  VkDebugUtilsMessageSeverityFlagBitsEXT debugUtilsMessageSeverityFlagBits =
      (VkDebugUtilsMessageSeverityFlagBitsEXT)(
      // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);

  VkDebugUtilsMessageTypeFlagBitsEXT debugUtilsMessageTypeFlagBits =
      (VkDebugUtilsMessageTypeFlagBitsEXT)(
      // VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);

  Instance* instance = new Instance(
      validationFeatureEnableList,
      validationFeatureDisableList,
      debugUtilsMessageSeverityFlagBits,
      debugUtilsMessageTypeFlagBits,
      "Demo Application",
      VK_MAKE_VERSION(1, 0, 0),
      {"VK_LAYER_KHRONOS_validation"},
      {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_xlib_surface"});

  std::cout << "Vulkan API " << instance->getVulkanVersionAPI().c_str()
      << std::endl;

  Surface::XlibSurfaceCreateInfoParam xlibSurfaceCreateInfoParam = {
    .displayPtr = displayPtr,
    .windowPtr = std::make_shared<Window>(window)
  };

  Surface* surface = new Surface(instance->getInstanceHandleRef(),
      Surface::Platform::Xlib,
      std::make_shared<Surface::XlibSurfaceCreateInfoParam>(
          xlibSurfaceCreateInfoParam));

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandleRef());

  VkPhysicalDevice activePhysicalDeviceHandle;
  for (VkPhysicalDevice deviceHandle : deviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties =
        Device::getPhysicalDeviceProperties(deviceHandle);

    if (physicalDeviceProperties.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {

      activePhysicalDeviceHandle = deviceHandle;
      break;
    }
  }

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList =
      Device::getQueueFamilyPropertiesList(activePhysicalDeviceHandle);

  uint32_t queueFamilyIndex = -1;
  for (uint32_t x = 0; x < queueFamilyPropertiesList.size(); x++) {
    if (queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
        Surface::checkPhysicalDeviceSurfaceSupport(activePhysicalDeviceHandle,
        x, surface->getSurfaceHandleRef())) {

      queueFamilyIndex = x;
      break;
    }
  }

  Device* device = new Device(activePhysicalDeviceHandle,
      {{0, queueFamilyIndex, 1, {1.0f}}},
      {},
      {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
      {});

  CommandPool* commandPool = new CommandPool(device->getDeviceHandleRef(),
      0,
      queueFamilyIndex);

  CommandBufferGroup* commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandleRef(), commandPool->getCommandPoolHandleRef(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

  VkSurfaceCapabilitiesKHR surfaceCapabilities =
      surface->getPhysicalDeviceSurfaceCapabilities(activePhysicalDeviceHandle);

  std::vector<VkSurfaceFormatKHR> surfaceFormatList =
      surface->getPhysicalDeviceSurfaceFormatList(activePhysicalDeviceHandle);

  std::vector<VkPresentModeKHR> presentModeList =
      surface->getPhysicalDeviceSurfacePresentModeList(activePhysicalDeviceHandle);

  Swapchain* swapchain = new Swapchain(device->getDeviceHandleRef(),
      0,
      surface->getSurfaceHandleRef(),
      surfaceCapabilities.minImageCount,
      surfaceFormatList[0].format,
      surfaceFormatList[0].colorSpace,
      surfaceCapabilities.currentExtent,
      1,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      surfaceCapabilities.currentTransform,
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      presentModeList[0],
      VK_TRUE,
      VK_NULL_HANDLE);

  // std::vector<VkAttachmentReference> attachmentReferenceList = {
  //   {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
  // };

  // RenderPass* renderPass = new RenderPass(device->getDeviceHandleRef(),
  //     (VkRenderPassCreateFlagBits)0,
  //     {{0,
  //         VK_FORMAT_R32G32B32A32_SFLOAT,
  //         VK_SAMPLE_COUNT_1_BIT,
  //         VK_ATTACHMENT_LOAD_OP_CLEAR,
  //         VK_ATTACHMENT_STORE_OP_STORE,
  //         VK_ATTACHMENT_LOAD_OP_DONT_CARE,
  //         VK_ATTACHMENT_STORE_OP_DONT_CARE,
  //         VK_IMAGE_LAYOUT_UNDEFINED,
  //         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}},
  //     {{0,
  //         VK_PIPELINE_BIND_POINT_GRAPHICS,
  //         0,
  //         NULL,
  //         1,
  //         attachmentReferenceList.data(),
  //         NULL,
  //         NULL,
  //         0,
  //         NULL,
  //     }},
  //     {});

  // XEvent event;
  // while (true) {
  //   XNextEvent(displayPtr, &event);

  //   if (event.type == KeyPress)
  //     break;
  // }

  // XCloseDisplay(displayPtr);

  return 0;
};