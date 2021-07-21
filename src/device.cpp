#include "vrk/device.h"

std::vector<VkPhysicalDevice> Device::getPhysicalDevices(
    VkInstance instanceHandle) {
  uint32_t physicalDeviceCount = 0;
  VkResult result = vkEnumeratePhysicalDevices(instanceHandle, 
      &physicalDeviceCount, NULL);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumeratePhysicalDevices");
    exit(1);
  }

  std::vector<VkPhysicalDevice> physicalDeviceList(physicalDeviceCount);
  result = vkEnumeratePhysicalDevices(instanceHandle, &physicalDeviceCount,
      physicalDeviceList.data());
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumeratePhysicalDevices");
    exit(1);
  }

  return physicalDeviceList;
}

Device::Device(VkInstance instanceHandle, VkPhysicalDevice physicalDeviceHandle,
      VkQueueFlagBits initialQueueFlagBits) {
  this->isActive = false;

  this->deviceHandle = VK_NULL_HANDLE;

  this->instanceHandle = instanceHandle;
  this->physicalDeviceHandle = physicalDeviceHandle;

  vkGetPhysicalDeviceProperties(physicalDeviceHandle,
      &this->physicalDeviceProperties);

  uint32_t extensionPropertiesCount = 0;
  VkResult result = vkEnumerateDeviceExtensionProperties(physicalDeviceHandle, NULL, &extensionPropertiesCount,
      NULL);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateDeviceExtensionProperties");
    exit(1);
  }

  this->extensionPropertiesList.resize(extensionPropertiesCount);
  result = vkEnumerateDeviceExtensionProperties(physicalDeviceHandle, NULL, &extensionPropertiesCount,
      this->extensionPropertiesList.data());
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateDeviceExtensionProperties");
    exit(1);
  }

  this->enabledExtensionNameList = {};

  uint32_t queueFamilyPropertyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle,
      &queueFamilyPropertyCount, NULL);
  this->queueFamilyPropertiesList.resize(queueFamilyPropertyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceHandle,
      &queueFamilyPropertyCount, this->queueFamilyPropertiesList.data());

  this->queueFlagBitsMap = {};
  for (uint32_t x = 0; x < this->queueFamilyPropertiesList.size(); x++) {
    if (this->queueFamilyPropertiesList[x].queueFlags & initialQueueFlagBits) {
      this->queueFlagBitsMap.insert(
          std::pair<uint32_t, VkQueueFlagBits>(x, initialQueueFlagBits));

      break;
    }
  }


  this->deviceQueueCreateInfoList = {};
}

Device::~Device() {
  vkDestroyDevice(this->deviceHandle, NULL);
}

std::vector<VkExtensionProperties> Device::getAvailableExtensionPropertiesList
    (std::string layerName) {

  if (layerName == "") {
    return this->extensionPropertiesList;
  }

  uint32_t extensionPropertiesCountLayer = 0;
  VkResult result = vkEnumerateDeviceExtensionProperties(this->physicalDeviceHandle, layerName.c_str(),
      &extensionPropertiesCountLayer, NULL);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateDeviceExtensionProperties");
    exit(1);
  }

  std::vector<VkExtensionProperties> extensionPropertiesListLayer(
      extensionPropertiesCountLayer);
  result = vkEnumerateDeviceExtensionProperties(this->physicalDeviceHandle, layerName.c_str(),
      &extensionPropertiesCountLayer, extensionPropertiesListLayer.data());
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateDeviceExtensionProperties");
    exit(1);
  }

  return extensionPropertiesListLayer;
}

bool Device::addExtension(std::string extensionName, std::string layerName) {
  bool foundExtension = false;

  if (layerName == "") {
    for (VkExtensionProperties extensionProperties :
        this->extensionPropertiesList) {
      if (extensionProperties.extensionName == extensionName) {
        this->enabledExtensionNameList.push_back(extensionName);
        foundExtension = true;
      }
    }
  }
  else {
    uint32_t extensionPropertiesCount = 0;
    VkResult result = vkEnumerateDeviceExtensionProperties(this->physicalDeviceHandle, layerName.c_str(),
        &extensionPropertiesCount, NULL);
    if (result != VK_SUCCESS) {
      PRINT_RETURN_CODE(stderr, result,
          "vkEnumerateDeviceExtensionProperties");
      exit(1);
    }

    std::vector<VkExtensionProperties> extensionPropertyList(
        extensionPropertiesCount);
    result = vkEnumerateDeviceExtensionProperties(this->physicalDeviceHandle, layerName.c_str(),
        &extensionPropertiesCount, extensionPropertyList.data());
    if (result != VK_SUCCESS) {
      PRINT_RETURN_CODE(stderr, result,
          "vkEnumerateDeviceExtensionProperties");
      exit(1);
    }

    for (VkExtensionProperties extensionProperties :
        extensionPropertyList) {
      if (extensionProperties.extensionName == extensionName) {
        this->enabledExtensionNameList.push_back(extensionName);
        foundExtension = true;
      }
    }
  }

  return foundExtension;
}

bool Device::addDeviceQueue(VkQueueFlagBits queueFlagBits) {
  for (uint32_t x = 0; x < this->queueFamilyPropertiesList.size(); x++) {
    if (this->queueFamilyPropertiesList[x].queueFlags & queueFlagBits &&
        this->queueFlagBitsMap.find(x) == this->queueFlagBitsMap.end()) {
      this->queueFlagBitsMap.insert(
          std::pair<uint32_t, VkQueueFlagBits>(x, queueFlagBits));

      return true;
    }
  }

  return false;
}

void Device::activate() {
  if (this->isActive) {
    PRINT_MESSAGE(stderr, "Device is already active");
    return;
  }

  for (auto const& x : this->queueFlagBitsMap) {
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .queueFamilyIndex = x.first,
      .queueCount = 1,
      .pQueuePriorities = &queuePriority,
    };
    this->deviceQueueCreateInfoList.push_back(deviceQueueCreateInfo);
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
    .queueCreateInfoCount = (uint32_t)this->deviceQueueCreateInfoList.size(),
    .pQueueCreateInfos = this->deviceQueueCreateInfoList.data(),
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = NULL,
    .enabledExtensionCount = (uint32_t)enabledExtensionNameList.size(),
    .ppEnabledExtensionNames = enabledExtensionNamesUnsafe,
    .pEnabledFeatures = NULL,
  };

  VkResult result = vkCreateDevice(this->physicalDeviceHandle, 
      &deviceCreateInfo, NULL, &this->deviceHandle);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkCreateDevice");
    exit(1);
  }

  free(enabledExtensionNamesUnsafe);

  this->isActive = true;
}