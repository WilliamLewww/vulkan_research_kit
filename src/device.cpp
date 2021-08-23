#include "vrk/device.h"

std::vector<VkPhysicalDevice> Device::getPhysicalDevices(
    VkInstance& instanceHandleRef) {

  uint32_t physicalDeviceCount = 0;

  VkResult result = vkEnumeratePhysicalDevices(instanceHandleRef,
      &physicalDeviceCount, NULL);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumeratePhysicalDevices");
  }

  std::vector<VkPhysicalDevice> physicalDeviceList(physicalDeviceCount);

  result = vkEnumeratePhysicalDevices(instanceHandleRef, &physicalDeviceCount,
      physicalDeviceList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumeratePhysicalDevices");
  }

  return physicalDeviceList;
}

VkPhysicalDeviceProperties Device::getPhysicalDeviceProperties(
    VkPhysicalDevice& physicalDeviceHandleRef) {

  VkPhysicalDeviceProperties physicalDeviceProperties;

  vkGetPhysicalDeviceProperties(physicalDeviceHandleRef,
      &physicalDeviceProperties);

  return physicalDeviceProperties;
}

std::vector<VkQueueFamilyProperties> Device::getQueueFamilyPropertiesList(
    VkPhysicalDevice& physicalDeviceHandleRef) {

  uint32_t queueFamilyPropertyCount = 0;

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandleRef,
      &queueFamilyPropertyCount, NULL);

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList(
      queueFamilyPropertyCount);

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandleRef,
      &queueFamilyPropertyCount, queueFamilyPropertiesList.data());

  return queueFamilyPropertiesList;
}

VkImageFormatProperties Device::getPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice& physicalDeviceHandleRef,
    VkFormat format,
    VkImageType imageType,
    VkImageTiling imageTiling,
    VkImageUsageFlags imageUsageFlags,
    VkImageCreateFlags imageCreateFlags) {

  VkImageFormatProperties imageFormatProperties = {};

  VkResult result = vkGetPhysicalDeviceImageFormatProperties(
      physicalDeviceHandleRef, format, imageType, imageTiling, imageUsageFlags,
      imageCreateFlags, &imageFormatProperties);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkGetPhysicalDeviceImageFormatProperties");
  }

  return imageFormatProperties;
}

VkPhysicalDeviceMemoryProperties Device::getPhysicalDeviceMemoryProperties(
    VkPhysicalDevice& physicalDeviceHandleRef) {

  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

  vkGetPhysicalDeviceMemoryProperties(physicalDeviceHandleRef,
      &physicalDeviceMemoryProperties);

  return physicalDeviceMemoryProperties;
}

Device::Device(VkPhysicalDevice& physicalDeviceHandleRef,
    std::vector<DeviceQueueCreateInfoParam> deviceQueueCreateInfoParamList,
    std::vector<std::string> enabledLayerNameList,
    std::vector<std::string> enabledExtensionNameList,
    std::vector<VkPhysicalDeviceFeatures> physicalDeviceFeaturesList) {

  this->deviceHandle = VK_NULL_HANDLE;

  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList;

  for (DeviceQueueCreateInfoParam& deviceQueueCreateInfoParam :
      deviceQueueCreateInfoParamList) {

    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = NULL,
      .flags = deviceQueueCreateInfoParam.deviceQueueCreateFlags,
      .queueFamilyIndex = deviceQueueCreateInfoParam.queueFamilyIndex,
      .queueCount = deviceQueueCreateInfoParam.queueCount,
      .pQueuePriorities = deviceQueueCreateInfoParam.queuePriorityList.data()
    };

    deviceQueueCreateInfoList.push_back(deviceQueueCreateInfo);
  }

  const char** enabledLayerNameBuffer = (const char**)malloc(
      sizeof(const char*) * enabledLayerNameList.size());

  for (uint32_t x = 0; x < enabledLayerNameList.size(); x++) {
    enabledLayerNameBuffer[x] = enabledLayerNameList[x].c_str();
  }

  const char** enabledExtensionNameBuffer = (const char**)malloc(
      sizeof(const char*) * enabledExtensionNameList.size());

  for (uint32_t x = 0; x < enabledExtensionNameList.size(); x++) {
    enabledExtensionNameBuffer[x] = enabledExtensionNameList[x].c_str();
  }

  VkDeviceCreateInfo deviceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueCreateInfoCount = (uint32_t)deviceQueueCreateInfoList.size(),
    .pQueueCreateInfos = deviceQueueCreateInfoList.data(),
    .enabledLayerCount = (uint32_t)enabledLayerNameList.size(),
    .ppEnabledLayerNames = enabledLayerNameBuffer,
    .enabledExtensionCount = (uint32_t)enabledExtensionNameList.size(),
    .ppEnabledExtensionNames = enabledExtensionNameBuffer,
    .pEnabledFeatures = physicalDeviceFeaturesList.data()
  };

  free(enabledExtensionNameBuffer);
  free(enabledLayerNameBuffer);

  VkResult result = vkCreateDevice(physicalDeviceHandleRef, &deviceCreateInfo,
      NULL, &this->deviceHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateDevice");
  }

  this->queueFamilyList = {};

  for (DeviceQueueCreateInfoParam& deviceQueueCreateInfoParam :
      deviceQueueCreateInfoParamList) {

    QueueFamily queueFamily = {
      .index = deviceQueueCreateInfoParam.queueFamilyIndex,
      .queueHandleList =
          std::vector<VkQueue>(deviceQueueCreateInfoParam.queueCount,
          VK_NULL_HANDLE)
    };

    for (uint32_t x = 0; x < deviceQueueCreateInfoParam.queueCount; x++) {
      vkGetDeviceQueue(this->deviceHandle,
          deviceQueueCreateInfoParam.queueFamilyIndex, x,
          &queueFamily.queueHandleList[x]);
    }

    this->queueFamilyList.push_back(queueFamily);
  }
}

Device::~Device() {
  vkDestroyDevice(this->deviceHandle, NULL);
}

VkDevice& Device::getDeviceHandleRef() {
  return this->deviceHandle;
}

VkQueue& Device::getQueueHandleRef(uint32_t queueFamilyIndex,
    uint32_t queueIndex) {

  return this->queueFamilyList[queueFamilyIndex].queueHandleList[queueIndex];
}