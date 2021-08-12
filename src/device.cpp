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

Device::Device() {

}

Device::~Device() {

}