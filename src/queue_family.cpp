#include "vrk/queue_family.h"

QueueFamily::QueueFamily(uint32_t queueFamilyIndex, uint32_t queueCount,
    float queuePriority) {

  this->isActive = false;

  this->queueHandleList = std::vector<VkQueue>(queueCount, VK_NULL_HANDLE);

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

uint32_t QueueFamily::getFamilyIndex() {
  return this->deviceQueueCreateInfo.queueFamilyIndex;
}

uint32_t QueueFamily::getQueueCount() {
  return this->deviceQueueCreateInfo.queueCount;
}

VkDeviceQueueCreateInfo QueueFamily::getDeviceQueueCreateInfo() {
  return this->deviceQueueCreateInfo;
}

void QueueFamily::activate(VkDevice* deviceHandlePtr) {
  if (this->isActive) {
    PRINT_MESSAGE(std::cerr, "Queue family is already active");
    return;
  }

  for (uint32_t x = 0; x < this->deviceQueueCreateInfo.queueCount; x++) {
    vkGetDeviceQueue(*deviceHandlePtr,
        this->deviceQueueCreateInfo.queueFamilyIndex, x,
        &this->queueHandleList[x]);
  }

  this->isActive = true;
}

VkQueue* QueueFamily::getQueueHandlePtr(uint32_t index) {
  return &this->queueHandleList[index];
}

std::ostream& operator<<(std::ostream& os, const QueueFamily& queueFamily) {
  os << "queue family " << 
      queueFamily.deviceQueueCreateInfo.queueFamilyIndex << 
      ": " << &queueFamily;

  for (uint32_t x = 0; x < queueFamily.queueHandleList.size(); x++) {
    os << std::endl;
    os << "  queue handle " << x << ": " << queueFamily.queueHandleList[x];
  }

  return os;
}