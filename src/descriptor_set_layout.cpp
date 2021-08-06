#include "vrk/descriptor_set_layout.h"

DescriptorSetLayout::DescriptorSetLayout(VkDevice* deviceHandlePtr,
    uint32_t initialBinding, 
    VkDescriptorType initialDescriptorType,
    uint32_t initialDescriptorCount, 
    VkShaderStageFlags initialShaderStageFlags,
    std::vector<VkSampler>* initialImmutableSamplerHandleListPtr) :
    Component("descriptor set layout") {

  this->descriptorSetLayoutHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;

  this->descriptorSetLayoutBindingList = {};

  VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {
    .binding = initialBinding,
    .descriptorType = initialDescriptorType,
    .descriptorCount = initialDescriptorCount,
    .stageFlags = initialShaderStageFlags,
    .pImmutableSamplers = NULL
  };

  if (initialImmutableSamplerHandleListPtr != NULL) {
    descriptorSetLayoutBinding.pImmutableSamplers =
        initialImmutableSamplerHandleListPtr->data();
  }

  this->descriptorSetLayoutBindingList.push_back(descriptorSetLayoutBinding);
}

DescriptorSetLayout::~DescriptorSetLayout() {

}

void DescriptorSetLayout::addSetLayoutBinding(uint32_t binding,
    VkDescriptorType descriptorType, 
    uint32_t descriptorCount,
    VkShaderStageFlags shaderStageFlags,
    std::vector<VkSampler>* immutableSamplerHandleListPtr) {

  VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {
    .binding = binding,
    .descriptorType = descriptorType,
    .descriptorCount = descriptorCount,
    .stageFlags = shaderStageFlags,
    .pImmutableSamplers = NULL
  };

  if (immutableSamplerHandleListPtr != NULL) {
    descriptorSetLayoutBinding.pImmutableSamplers =
        immutableSamplerHandleListPtr->data();
  }

  this->descriptorSetLayoutBindingList.push_back(descriptorSetLayoutBinding);
}

bool DescriptorSetLayout::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .bindingCount = (uint32_t)this->descriptorSetLayoutBindingList.size(),
    .pBindings = this->descriptorSetLayoutBindingList.data()
  };

  VkResult result = vkCreateDescriptorSetLayout(*this->deviceHandlePtr,
      &descriptorSetLayoutCreateInfo, NULL, &this->descriptorSetLayoutHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateDescriptorSetLayout");
  }

  return true;
}