#pragma once

#include "vrk/helper.h"
#include "vrk/queue_family.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

class Device : public Component {
private:
  VkDevice deviceHandle;
  std::vector<QueueFamily> queueFamilyList;

  VkInstance* instanceHandlePtr;
  VkPhysicalDevice* physicalDeviceHandlePtr;

  std::vector<VkExtensionProperties> extensionPropertiesList;
  std::vector<std::string> enabledExtensionNameList;
public:
  static std::vector<VkPhysicalDevice> getPhysicalDevices(
      VkInstance* instanceHandlePtr);

  static VkPhysicalDeviceProperties getPhysicalDeviceProperties(
      VkPhysicalDevice* physicalDeviceHandlePtr);

  static std::vector<VkQueueFamilyProperties> getQueueFamilyPropertiesList(
      VkPhysicalDevice* physicalDeviceHandlePtr);

  static VkBool32 checkQueueFamilyPresentSupported(
      VkPhysicalDevice* physicalDeviceHandlePtr, uint32_t queueFamilyIndex,
      VkSurfaceKHR* surfaceHandle);

  static VkImageFormatProperties getPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice* physicalDeviceHandlePtr, VkFormat format,
    VkImageType imageType, VkImageTiling imageTiling,
    VkImageUsageFlags imageUsageFlags, VkImageCreateFlags imageCreateFlags);

  Device(VkInstance* instanceHandlePtr,
      VkPhysicalDevice* physicalDeviceHandlePtr,
      uint32_t initialQueueFamilyIndex, uint32_t initialQueueCount,
      float initialQueuePriority = 1.0f);

  ~Device();

  std::vector<VkExtensionProperties> getAvailableExtensionPropertiesList();

  bool addExtension(std::string extensionName);

  void addQueueFamily(uint32_t queueFamilyIndex, uint32_t queueCount,
      float queuePriority = 1.0f);

  bool activate();

  VkDevice* getDeviceHandlePtr();

  VkQueue* getQueueHandlePtr(uint32_t queueFamilyIndex, uint32_t queueIndex);

  friend std::ostream& operator<<(std::ostream& os, const Device& device);
};