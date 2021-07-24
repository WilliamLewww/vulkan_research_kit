#include "vrk/queue_family.h"

QueueFamily::QueueFamily(uint32_t queueFamilyIndex, uint32_t queueCount,
    float queuePriority) {

  this->queuePriority = queuePriority;

  this->deviceQueueCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueFamilyIndex = queueFamilyIndex,
    .queueCount = queueCount,
    .pQueuePriorities = &queuePriority,
  };
}

QueueFamily::~QueueFamily() {

}

VkDeviceQueueCreateInfo QueueFamily::getDeviceQueueCreateInfo() {
  return this->deviceQueueCreateInfo;
}