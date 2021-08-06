#include "vrk/device.h"

std::vector<VkPhysicalDevice> Device::getPhysicalDevices(
    VkInstance* instanceHandlePtr) {

  uint32_t physicalDeviceCount = 0;
  VkResult result = vkEnumeratePhysicalDevices(*instanceHandlePtr,
      &physicalDeviceCount, NULL);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumeratePhysicalDevices");
  }

  std::vector<VkPhysicalDevice> physicalDeviceList(physicalDeviceCount);
  result = vkEnumeratePhysicalDevices(*instanceHandlePtr, &physicalDeviceCount,
      physicalDeviceList.data());
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumeratePhysicalDevices");
  }

  return physicalDeviceList;
}

VkPhysicalDeviceProperties Device::getPhysicalDeviceProperties(
    VkPhysicalDevice* physicalDeviceHandlePtr) {

  VkPhysicalDeviceProperties physicalDeviceProperties;

  vkGetPhysicalDeviceProperties(*physicalDeviceHandlePtr,
      &physicalDeviceProperties);

  return physicalDeviceProperties;
}

std::vector<VkQueueFamilyProperties> Device::getQueueFamilyPropertiesList(
    VkPhysicalDevice* physicalDeviceHandlePtr) {

  uint32_t queueFamilyPropertyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(*physicalDeviceHandlePtr,
      &queueFamilyPropertyCount, NULL);

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList(
      queueFamilyPropertyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(*physicalDeviceHandlePtr,
      &queueFamilyPropertyCount, queueFamilyPropertiesList.data());

  return queueFamilyPropertiesList;
}

VkBool32 Device::checkQueueFamilyPresentSupported(
    VkPhysicalDevice* physicalDeviceHandlePtr,
    uint32_t queueFamilyIndex,
    VkSurfaceKHR* surfaceHandle) {

  VkBool32 isPresentSupported = false;

  VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
      *physicalDeviceHandlePtr, queueFamilyIndex, *surfaceHandle,
      &isPresentSupported);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkGetPhysicalDeviceSurfaceSupportKHR");
  }

  return isPresentSupported;
}

VkImageFormatProperties Device::getPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice* physicalDeviceHandlePtr,
    VkFormat format,
    VkImageType imageType,
    VkImageTiling imageTiling,
    VkImageUsageFlags imageUsageFlags,
    VkImageCreateFlags imageCreateFlags) {

  VkImageFormatProperties imageFormatProperties = {};

  VkResult result = vkGetPhysicalDeviceImageFormatProperties(
      *physicalDeviceHandlePtr, format, imageType, imageTiling, imageUsageFlags,
      imageCreateFlags, &imageFormatProperties);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkGetPhysicalDeviceImageFormatProperties");
  }

  return imageFormatProperties;
}

VkPhysicalDeviceMemoryProperties Device::getPhysicalDeviceMemoryProperties(
    VkPhysicalDevice* physicalDeviceHandlePtr) {

  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
  vkGetPhysicalDeviceMemoryProperties(*physicalDeviceHandlePtr,
      &physicalDeviceMemoryProperties);

  return physicalDeviceMemoryProperties;
}

Device::Device(VkInstance* instanceHandlePtr,
    VkPhysicalDevice* physicalDeviceHandlePtr,
    uint32_t initialQueueFamilyIndex,
    uint32_t initialQueueCount,
    float initialQueuePriority) :
    Component("device") {

  this->deviceHandle = VK_NULL_HANDLE;

  this->instanceHandlePtr = instanceHandlePtr;
  this->physicalDeviceHandlePtr = physicalDeviceHandlePtr;

  uint32_t extensionPropertiesCount = 0;
  VkResult result = vkEnumerateDeviceExtensionProperties(
      *physicalDeviceHandlePtr, NULL, &extensionPropertiesCount, NULL);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result,
        "vkEnumerateDeviceExtensionProperties");
  }

  this->extensionPropertiesList.resize(extensionPropertiesCount);
  result = vkEnumerateDeviceExtensionProperties(*physicalDeviceHandlePtr, NULL,
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

std::vector<VkExtensionProperties>
    Device::getAvailableExtensionPropertiesList() {

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

void Device::addQueueFamily(uint32_t queueFamilyIndex,
    uint32_t queueCount,
    float queuePriority) {

  this->queueFamilyList.push_back(QueueFamily(queueFamilyIndex,
      queueCount, queuePriority));
}

bool Device::activate() {
  if (!Component::activate()) {
    return false;
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

  VkResult result = vkCreateDevice(*this->physicalDeviceHandlePtr,
      &deviceCreateInfo, NULL, &this->deviceHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateDevice");
  }

  free(enabledExtensionNamesUnsafe);

  for (QueueFamily& queueFamily : this->queueFamilyList) {
    queueFamily.activate(&this->deviceHandle);
  }

  return true;
}

VkDevice* Device::getDeviceHandlePtr() {
  return &this->deviceHandle;
}

VkQueue* Device::getQueueHandlePtr(uint32_t queueFamilyIndex,
    uint32_t queueIndex) {

  return this->queueFamilyList[queueFamilyIndex].getQueueHandlePtr(queueIndex);
}