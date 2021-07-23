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

  std::vector<VkExtensionProperties> extensionPropertiesList;
  std::vector<std::string> enabledExtensionNameList;
public:
  static std::vector<VkPhysicalDevice> getPhysicalDevices(
      VkInstance instanceHandle);

  static VkPhysicalDeviceProperties getPhysicalDeviceProperties(
      VkPhysicalDevice physicalDeviceHandle);

  static std::vector<VkQueueFamilyProperties> getQueueFamilyPropertiesList(
      VkPhysicalDevice physicalDeviceHandle);

  Device(VkInstance instanceHandle, VkPhysicalDevice physicalDeviceHandle,
      uint32_t initialQueueFamilyIndex, int queueCount);

  ~Device();

  std::vector<VkExtensionProperties> getAvailableExtensionPropertiesList(
      std::string layerName = "");

  bool addExtension(std::string extensionName, std::string layerName = "");

  void activate();

  VkDevice getDeviceHandle();
};