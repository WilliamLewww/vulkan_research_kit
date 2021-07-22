#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>

class Device {
private:
  bool isActive;

  VkDevice deviceHandle;

  VkInstance instanceHandle;
  VkPhysicalDevice physicalDeviceHandle;

  VkPhysicalDeviceProperties physicalDeviceProperties;

  std::vector<VkExtensionProperties> extensionPropertiesList;
  std::vector<std::string> enabledExtensionNameList;

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList;
  std::multimap<uint32_t, VkQueueFlagBits> queueFlagBitsMap;
  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList;
public:
  static std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instanceHandle);

  Device(VkInstance instanceHandle, VkPhysicalDevice physicalDeviceHandle,
      VkQueueFlagBits initialQueueFlagBits);

  ~Device();

  std::vector<VkExtensionProperties> getAvailableExtensionPropertiesList(
      std::string layerName = "");

  bool addExtension(std::string extensionName, std::string layerName = "");

  bool addDeviceQueue(VkQueueFlagBits queueFlagBits);

  void activate();
};