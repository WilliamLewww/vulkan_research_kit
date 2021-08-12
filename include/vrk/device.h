#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>

class Device {
public:
  static std::vector<VkPhysicalDevice> getPhysicalDevices(
      VkInstance* instanceHandlePtr);

  static VkPhysicalDeviceProperties getPhysicalDeviceProperties(
      VkPhysicalDevice* physicalDeviceHandlePtr);

  static std::vector<VkQueueFamilyProperties> getQueueFamilyPropertiesList(
      VkPhysicalDevice* physicalDeviceHandlePtr);

  Device();

  ~Device();
private:
};