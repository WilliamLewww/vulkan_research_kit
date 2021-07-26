#include "vrk/instance.h"
#include "vrk/device.h"
#include "vrk/command_pool.h"
#include "vrk/command_buffer_group.h"

int main() {
  Instance* instance = new Instance();
  std::cout << "Vulkan API " << instance->getVulkanVersionAPI().c_str()
      << std::endl;

  instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
  instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT);
  instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);

  instance->setDebugUtilsMessageSeverityFlagBits(
      (VkDebugUtilsMessageSeverityFlagBitsEXT)
      (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT));

  instance->setDebugUtilsMessageTypeFlagBits(
    (VkDebugUtilsMessageTypeFlagBitsEXT)
    (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT));

  instance->addLayer("VK_LAYER_KHRONOS_validation");
  instance->addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  instance->activate();

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandlePtr());

  VkPhysicalDevice activePhysicalDevice;
  for (VkPhysicalDevice deviceHandle : deviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties =
        Device::getPhysicalDeviceProperties(&deviceHandle);

    if (physicalDeviceProperties.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      activePhysicalDevice = deviceHandle;
      break;
    }
  }

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList =
      Device::getQueueFamilyPropertiesList(&activePhysicalDevice);

  uint32_t queueFamilyIndex = -1;
  for (uint32_t x = 0; x < queueFamilyPropertiesList.size(); x++) {
    if (queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndex = x;
    }
  }

  Device* device = new Device(instance->getInstanceHandlePtr(),
      &activePhysicalDevice, queueFamilyIndex, 1);

  device->activate();

  CommandPool* commandPool = new CommandPool(device->getDeviceHandlePtr(),
      queueFamilyIndex);
  commandPool->activate();

  CommandBufferGroup* commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandlePtr(), commandPool->getCommandPoolHandlePtr(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
  commandBufferGroup->activate();

  std::cout << *instance << std::endl;
  std::cout << std::endl;
  std::cout << *device << std::endl;
  std::cout << std::endl;
  std::cout << *commandPool << std::endl;
  std::cout << std::endl;
  std::cout << *commandBufferGroup << std::endl;

  delete commandBufferGroup;
  delete commandPool;
  delete device;
  delete instance;

  return 0;
}
