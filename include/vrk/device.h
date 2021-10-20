#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class Device {
public:
  struct DeviceQueueCreateInfoParam {
    VkDeviceQueueCreateFlags deviceQueueCreateFlags;
    uint32_t queueFamilyIndex;
    uint32_t queueCount;
    std::vector<float> queuePriorityList;
  };

  struct QueueFamily {
    uint32_t index;
    std::vector<VkQueue> queueHandleList;
  };

  static std::vector<VkPhysicalDevice>
  getPhysicalDevices(VkInstance &instanceHandleRef);

  static VkPhysicalDeviceProperties
  getPhysicalDeviceProperties(VkPhysicalDevice &physicalDeviceHandleRef);

  static std::vector<VkQueueFamilyProperties>
  getQueueFamilyPropertiesList(VkPhysicalDevice &physicalDeviceHandleRef);

  static VkImageFormatProperties getPhysicalDeviceImageFormatProperties(
      VkPhysicalDevice &physicalDeviceHandleRef, VkFormat format,
      VkImageType imageType, VkImageTiling imageTiling,
      VkImageUsageFlags imageUsageFlags, VkImageCreateFlags imageCreateFlags);

  static VkPhysicalDeviceMemoryProperties
  getPhysicalDeviceMemoryProperties(VkPhysicalDevice &physicalDeviceHandleRef);

  Device(VkPhysicalDevice &physicalDeviceHandleRef,
         std::vector<DeviceQueueCreateInfoParam> deviceQueueCreateInfoParamList,
         std::vector<std::string> enabledLayerNameList,
         std::vector<std::string> enabledExtensionNameList,
         std::shared_ptr<VkPhysicalDeviceFeatures> physicalDeviceFeaturesPtr,
         std::vector<void *> deviceCreateInfoChainList = {});

  ~Device();

  void waitIdle();

  VkDevice &getDeviceHandleRef();

  VkQueue &getQueueHandleRef(uint32_t queueFamilyIndex, uint32_t queueIndex);

private:
  VkDevice deviceHandle;

  std::vector<QueueFamily> queueFamilyList;
};
