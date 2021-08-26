#include "vrk/descriptor_pool.h"

DescriptorPool::DescriptorPool(VkDevice& deviceHandleRef,
    VkDescriptorPoolCreateFlags descriptorPoolCreateFlags,
    uint32_t maxSets,
    std::vector<VkDescriptorPoolSize> descriptorPoolSizeList) :
    deviceHandleRef(deviceHandleRef) {

  this->descriptorPoolHandle = VK_NULL_HANDLE;

  VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pNext = NULL,
    .flags = descriptorPoolCreateFlags,
    .maxSets = maxSets,
    .poolSizeCount = (uint32_t)descriptorPoolSizeList.size(),
    .pPoolSizes = descriptorPoolSizeList.data()
  };

  VkResult result = vkCreateDescriptorPool(deviceHandleRef,
      &descriptorPoolCreateInfo, NULL, &this->descriptorPoolHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateDescriptorPool");
  }
}

DescriptorPool::~DescriptorPool() {
  vkDestroyDescriptorPool(this->deviceHandleRef, this->descriptorPoolHandle,
      NULL);
}

VkDescriptorPool& DescriptorPool::getDescriptorPoolHandleRef() {
  return this->descriptorPoolHandle;
}