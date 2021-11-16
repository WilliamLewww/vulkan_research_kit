#include "vrk/device.h"
#include "vrk/instance.h"

#include "vrk/dynamic_rendering/dynamic_rendering.h"

int main() {
  std::vector<VkValidationFeatureEnableEXT> validationFeatureEnableList = {
      // VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
      VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
      VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT};

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

  std::vector<std::string> instanceExtensionList = {
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
  instanceExtensionList.insert(
      instanceExtensionList.end(),
      DYNAMIC_RENDERING_REQUIRED_INSTANCE_EXTENSION_LIST.begin(),
      DYNAMIC_RENDERING_REQUIRED_INSTANCE_EXTENSION_LIST.end());
  instanceExtensionList.erase(
      unique(instanceExtensionList.begin(), instanceExtensionList.end()),
      instanceExtensionList.end());

  Instance *instance = new Instance(
      validationFeatureEnableList, validationFeatureDisableList,
      debugUtilsMessageSeverityFlagBits, debugUtilsMessageTypeFlagBits,
      "Demo Application", VK_MAKE_VERSION(1, 0, 0),
      {"VK_LAYER_KHRONOS_validation"}, instanceExtensionList);

  std::cout << "Vulkan API " << instance->getVulkanVersionAPI().c_str()
            << std::endl;

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandleRef());

  VkPhysicalDevice activePhysicalDeviceHandle = deviceHandleList[0];
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
    if (queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndex = x;
      break;
    }
  }

  std::vector<std::string> deviceExtensionList;
  deviceExtensionList.insert(
      deviceExtensionList.end(),
      DYNAMIC_RENDERING_REQUIRED_DEVICE_EXTENSION_LIST.begin(),
      DYNAMIC_RENDERING_REQUIRED_DEVICE_EXTENSION_LIST.end());
  deviceExtensionList.erase(
      unique(deviceExtensionList.begin(), deviceExtensionList.end()),
      deviceExtensionList.end());

  Device *device =
      new Device(activePhysicalDeviceHandle, {{0, queueFamilyIndex, 1, {1.0f}}},
                 {}, deviceExtensionList, NULL);

  return 0;
}
