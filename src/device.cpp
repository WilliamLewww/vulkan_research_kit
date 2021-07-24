#include "vrk/device.h"

std::vector<VkPhysicalDevice> Device::getPhysicalDevices(
    VkInstance instanceHandle) {

  if (instanceHandle == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid instance handle");
  }

  uint32_t physicalDeviceCount = 0;
  VkResult result = vkEnumeratePhysicalDevices(instanceHandle,
      &physicalDeviceCount, NULL);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumeratePhysicalDevices");
  }

  std::vector<VkPhysicalDevice> physicalDeviceList(physicalDeviceCount);
  result = vkEnumeratePhysicalDevices(instanceHandle, &physicalDeviceCount,
      physicalDeviceList.data());
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumeratePhysicalDevices");
  }

  return physicalDeviceList;
}

VkPhysicalDeviceProperties Device::getPhysicalDeviceProperties(
    VkPhysicalDevice physicalDeviceHandle) {

  if (physicalDeviceHandle == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid physical device handle");
  }

  VkPhysicalDeviceProperties physicalDeviceProperties;

  vkGetPhysicalDeviceProperties(physicalDeviceHandle,
      &physicalDeviceProperties);

  return physicalDeviceProperties;
}

std::vector<VkQueueFamilyProperties> Device::getQueueFamilyPropertiesList(
    VkPhysicalDevice physicalDeviceHandle) {

  if (physicalDeviceHandle == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid physical device handle");
  }

  uint32_t queueFamilyPropertyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle,
      &queueFamilyPropertyCount, NULL);

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList(
      queueFamilyPropertyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle,
      &queueFamilyPropertyCount, queueFamilyPropertiesList.data());

  return queueFamilyPropertiesList;
}

Device::Device(VkInstance instanceHandle, VkPhysicalDevice physicalDeviceHandle,
    uint32_t initialQueueFamilyIndex, uint32_t initialQueueCount, 
    float initialQueuePriority) {

  if (instanceHandle == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid instance handle");
  }

  if (physicalDeviceHandle == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid physical device handle");
  }

  this->isActive = false;

  this->deviceHandle = VK_NULL_HANDLE;

  this->instanceHandle = instanceHandle;
  this->physicalDeviceHandle = physicalDeviceHandle;

  uint32_t extensionPropertiesCount = 0;
  VkResult result = vkEnumerateDeviceExtensionProperties(physicalDeviceHandle,
      NULL, &extensionPropertiesCount, NULL);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result,
        "vkEnumerateDeviceExtensionProperties");
  }

  this->extensionPropertiesList.resize(extensionPropertiesCount);
  result = vkEnumerateDeviceExtensionProperties(physicalDeviceHandle, NULL,
      &extensionPropertiesCount, this->extensionPropertiesList.data());
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result,
        "vkEnumerateDeviceExtensionProperties");
  }

  this->enabledExtensionNameList = {};

  this->queueFamilyList = {};
  this->queueFamilyList.push_back(QueueFamily(initialQueueFamilyIndex, 
      initialQueueCount, initialQueuePriority));
}

Device::~Device() {
  vkDestroyDevice(this->deviceHandle, NULL);
}

std::vector<VkExtensionProperties> Device::getAvailableExtensionPropertiesList
    () {

  return this->extensionPropertiesList;
}

bool Device::addExtension(std::string extensionName) {
  bool foundExtension = false;

  if (std::find_if(
      std::begin(this->extensionPropertiesList),
      std::end(this->extensionPropertiesList),
      [&](const VkExtensionProperties& x)
      { return x.extensionName == extensionName; }) != 
      std::end(this->extensionPropertiesList)) {

    this->enabledExtensionNameList.push_back(extensionName);
    foundExtension = true;
  }

  return foundExtension;
}

void Device::addQueue(uint32_t initialQueueFamilyIndex, 
    uint32_t initialQueueCount, float initialQueuePriority) {

  this->queueFamilyList.push_back(QueueFamily(initialQueueFamilyIndex, 
      initialQueueCount, initialQueuePriority));
}

void Device::activate() {
  if (this->isActive) {
    PRINT_MESSAGE(std::cerr, "Device is already active");
    return;
  }

  std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfoList(
      queueFamilyList.size());

  for (uint32_t x = 0; x < queueFamilyList.size(); x++) {
    deviceQueueCreateInfoList[x] = 
        queueFamilyList[x].getDeviceQueueCreateInfo();
  }

  const char** enabledExtensionNamesUnsafe = (const char**)malloc(
      sizeof(const char*) * enabledExtensionNameList.size());

  for (uint32_t x = 0; x < enabledExtensionNameList.size(); x++) {
    enabledExtensionNamesUnsafe[x] = enabledExtensionNameList[x].c_str();
  }

  VkDeviceCreateInfo deviceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueCreateInfoCount = (uint32_t)deviceQueueCreateInfoList.size(),
    .pQueueCreateInfos = deviceQueueCreateInfoList.data(),
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = NULL,
    .enabledExtensionCount = (uint32_t)enabledExtensionNameList.size(),
    .ppEnabledExtensionNames = enabledExtensionNamesUnsafe,
    .pEnabledFeatures = NULL,
  };

  VkResult result = vkCreateDevice(this->physicalDeviceHandle,
      &deviceCreateInfo, NULL, &this->deviceHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateDevice");
  }

  free(enabledExtensionNamesUnsafe);

  this->isActive = true;
}

VkDevice Device::getDeviceHandle() {
  return this->deviceHandle;
}