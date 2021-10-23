#include "vrk/descriptor_set_layout.h"

DescriptorSetLayout::DescriptorSetLayout(
    VkDevice &deviceHandleRef,
    VkDescriptorSetLayoutCreateFlags descriptorSetLayoutCreateFlags,
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList)
    : deviceHandleRef(deviceHandleRef) {

  this->descriptorSetLayoutHandle = VK_NULL_HANDLE;

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .flags = descriptorSetLayoutCreateFlags,
      .bindingCount = (uint32_t)descriptorSetLayoutBindingList.size(),
      .pBindings = descriptorSetLayoutBindingList.data()};

  VkResult result = vkCreateDescriptorSetLayout(
      deviceHandleRef, &descriptorSetLayoutCreateInfo, NULL,
      &this->descriptorSetLayoutHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateDescriptorSetLayout");
  }
}

DescriptorSetLayout::~DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(this->deviceHandleRef,
                               this->descriptorSetLayoutHandle, NULL);
}

VkDescriptorSetLayout &DescriptorSetLayout::getDescriptorSetLayoutHandleRef() {
  return this->descriptorSetLayoutHandle;
}