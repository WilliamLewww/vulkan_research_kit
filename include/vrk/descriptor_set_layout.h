#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"

#include <vulkan/vulkan.h>

#include <vector>

class DescriptorSetLayout : public Component {
private:
  VkDescriptorSetLayout descriptorSetLayoutHandle;

  VkDevice* deviceHandlePtr;

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList;
public:
  DescriptorSetLayout(VkDevice* deviceHandlePtr,
      uint32_t initialBinding, 
      VkDescriptorType initialDescriptorType,
      uint32_t initialDescriptorCount,
      VkShaderStageFlags initialShaderStageFlags,
      std::vector<VkSampler>* initialImmutableSamplerHandleListPtr);

  ~DescriptorSetLayout();

  void addSetLayoutBinding(uint32_t binding, 
      VkDescriptorType descriptorType,
      uint32_t descriptorCount, 
      VkShaderStageFlags shaderStageFlags,
      std::vector<VkSampler>* immutableSamplerHandleListPtr);

  bool activate();
};