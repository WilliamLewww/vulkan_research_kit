#include "vrk/descriptor_pool.h"

DescriptorPool::DescriptorPool(VkDevice* deviceHandlePtr,
    VkDescriptorPoolCreateFlags descriptorPoolCreateFlags, uint32_t maxSets,
    std::vector<DescriptorPoolSize> descriptorPoolSizeList)
    : Component("descriptor pool") {

  this->descriptorPoolHandle = descriptorPoolHandle;

  this->deviceHandlePtr = deviceHandlePtr;

  this->descriptorPoolSizeList = {};
  for (uint32_t x = 0; x < descriptorPoolSizeList.size(); x++) {
    VkDescriptorPoolSize descriptorPoolSize = {
      .type = descriptorPoolSizeList[x].type,
      .descriptorCount = descriptorPoolSizeList[x].descriptorCount
    };

    this->descriptorPoolSizeList.push_back(descriptorPoolSize);
  }

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

bool DescriptorPool::activate() {
  if (!Component::activate()) {
    return true;
  }

  VkResult result = vkCreateDescriptorPool(*this->deviceHandlePtr,
      &this->descriptorPoolCreateInfo, NULL, &this->descriptorPoolHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateDescriptorPool");
  }

  return true;
}