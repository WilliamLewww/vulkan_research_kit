#include "vrk/descriptor_set_group.h"

DescriptorSetGroup::DescriptorSetGroup(VkDevice& deviceHandleRef,
    VkDescriptorPool& descriptorPoolHandleRef,
    std::vector<VkDescriptorSetLayout> descriptorSetLayoutList) :
    deviceHandleRef(deviceHandleRef),
    descriptorPoolHandleRef(descriptorPoolHandleRef) {

  this->descriptorSetHandleList = std::vector<VkDescriptorSet>(
      descriptorSetLayoutList.size(), VK_NULL_HANDLE);

  VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    .pNext = NULL,
    .descriptorPool = descriptorPoolHandleRef,
    .descriptorSetCount = (uint32_t)descriptorSetLayoutList.size(),
    .pSetLayouts = descriptorSetLayoutList.data()
  };

  VkResult result = vkAllocateDescriptorSets(deviceHandleRef,
      &descriptorSetAllocateInfo, this->descriptorSetHandleList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkAllocateDescriptorSets");
  }
}

DescriptorSetGroup::~DescriptorSetGroup() {
  VkResult result = vkFreeDescriptorSets(this->deviceHandleRef,
      this->descriptorPoolHandleRef, this->descriptorSetHandleList.size(),
      this->descriptorSetHandleList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkFreeDescriptorSets");
  }
}