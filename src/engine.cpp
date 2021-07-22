#include "vrk/engine.h"

Engine::Engine() {
  this->instance = new Instance();
  std::cout << "Vulkan API " << this->instance->getVulkanVersionAPI().c_str() 
      << std::endl;

  this->instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
  this->instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT);
  this->instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);

  this->instance->setDebugUtilsMessageSeverityFlagBits(
      (VkDebugUtilsMessageSeverityFlagBitsEXT)
      (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT));

  this->instance->setDebugUtilsMessageTypeFlagBits(
    (VkDebugUtilsMessageTypeFlagBitsEXT)
    (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT));

  this->instance->addLayer("VK_LAYER_KHRONOS_validation");
  this->instance->addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  this->instance->activate();

  std::vector<VkPhysicalDevice> deviceList =
      Device::getPhysicalDevices(this->instance->getInstanceHandle());

  this->device = new Device(this->instance->getInstanceHandle(), deviceList[0],
      VK_QUEUE_GRAPHICS_BIT);
  this->device->addDeviceQueue(VK_QUEUE_COMPUTE_BIT);

  this->device->activate();
}

Engine::~Engine() {
  delete this->device;
  delete this->instance;
}
