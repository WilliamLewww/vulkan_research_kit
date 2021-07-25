#include "vrk/device.h"

std::vector<VkPhysicalDevice> Device::getPhysicalDevices(
    VkInstance* instanceHandlePtr) {

  if (*instanceHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid instance handle");
  }

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

  if (*physicalDeviceHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid physical device handle");
  }

  VkPhysicalDeviceProperties physicalDeviceProperties;

  vkGetPhysicalDeviceProperties(*physicalDeviceHandlePtr,
      &physicalDeviceProperties);

  return physicalDeviceProperties;
}

std::vector<VkQueueFamilyProperties> Device::getQueueFamilyPropertiesList(
    VkPhysicalDevice* physicalDeviceHandlePtr) {

  if (*physicalDeviceHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid physical device handle");
  }

  uint32_t queueFamilyPropertyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(*physicalDeviceHandlePtr,
      &queueFamilyPropertyCount, NULL);

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList(
      queueFamilyPropertyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(*physicalDeviceHandlePtr,
      &queueFamilyPropertyCount, queueFamilyPropertiesList.data());

  return queueFamilyPropertiesList;
}

Device::Device(VkInstance* instanceHandlePtr,
    VkPhysicalDevice* physicalDeviceHandlePtr, uint32_t initialQueueFamilyIndex,
    uint32_t initialQueueCount, float initialQueuePriority) {

  if (*instanceHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid instance handle");
  }

  if (*physicalDeviceHandlePtr == VK_NULL_HANDLE) {
    throwExceptionMessage("Invalid physical device handle");
  }

  this->isActive = false;

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

void Device::addQueue(uint32_t queueFamilyIndex, uint32_t queueCount,
    float queuePriority) {

  this->queueFamilyList.push_back(QueueFamily(queueFamilyIndex,
      queueCount, queuePriority));
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

  VkResult result = vkCreateDevice(*this->physicalDeviceHandlePtr,
      &deviceCreateInfo, NULL, &this->deviceHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateDevice");
  }

  free(enabledExtensionNamesUnsafe);

  for (QueueFamily& queueFamily : this->queueFamilyList) {
    queueFamily.activate(&this->deviceHandle);
  }

  this->isActive = true;
}

VkDevice* Device::getDeviceHandlePtr() {
  return &this->deviceHandle;
}

VkQueue* Device::getQueueHandlePtr(uint32_t queueFamilyIndex, 
    uint32_t queueIndex) {

  return this->queueFamilyList[queueFamilyIndex].getQueueHandlePtr(queueIndex);
}

std::ostream& operator<<(std::ostream& os, const Device& device)
{
  os << "device: " << &device << std::endl;
  os << "  device handle: " << device.deviceHandle << std::endl;
  os << "  instance handle (ptr): " << *device.instanceHandlePtr << std::endl;
  os << "  physical device handle (ptr): " << 
      *device.physicalDeviceHandlePtr;

  for (QueueFamily queueFamily : device.queueFamilyList) {
    os << std::endl;
    os << queueFamily;
  }

  return os;
}