#include "vrk/descriptor_pool.h"

DescriptorPool::DescriptorPool(VkDevice* deviceHandlePtr,
    VkDescriptorPoolCreateFlags descriptorPoolCreateFlags, uint32_t maxSets,
    VkDescriptorType initialDescriptorType, uint32_t initialDescriptorCount) :
    Component("descriptor pool") {

  this->descriptorPoolHandle = descriptorPoolHandle;

  this->deviceHandlePtr = deviceHandlePtr;

  VkDescriptorPoolSize descriptorPoolSize = {
    .type = initialDescriptorType,
    .descriptorCount = initialDescriptorCount
  };

  this->descriptorPoolSizeList = {descriptorPoolSize};

  this->descriptorPoolCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pNext = NULL,
    .flags = descriptorPoolCreateFlags,
    .maxSets = maxSets,
    .poolSizeCount = (uint32_t)this->descriptorPoolSizeList.size(),
    .pPoolSizes = this->descriptorPoolSizeList.data()
  };
}

DescriptorPool::~DescriptorPool() {

}

void DescriptorPool::addDescriptorPoolSize(VkDescriptorType descriptorType,
    uint32_t descriptorCount) {

  VkDescriptorPoolSize descriptorPoolSize = {
    .type = descriptorType,
    .descriptorCount = descriptorCount
  };

  this->descriptorPoolSizeList.push_back(descriptorPoolSize);
}

bool DescriptorPool::activate() {
  if (!Component::activate()) {
    return true;
  }

  this->descriptorPoolCreateInfo.poolSizeCount =
      (uint32_t)this->descriptorPoolSizeList.size();

  this->descriptorPoolCreateInfo.pPoolSizes =
      this->descriptorPoolSizeList.data();

  VkResult result = vkCreateDescriptorPool(*this->deviceHandlePtr,
      &this->descriptorPoolCreateInfo, NULL, &this->descriptorPoolHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateDescriptorPool");
  }

  return true;
}