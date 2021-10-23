#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class DescriptorSetGroup {
public:
  struct WriteDescriptorSetParam {
    uint32_t dstDescriptorSetIndex;
    uint32_t dstBinding;
    uint32_t dstArrayElement;
    uint32_t descriptorCount;
    VkDescriptorType descriptorType;
    std::shared_ptr<VkDescriptorImageInfo> descriptorImageInfoPtr;
    std::shared_ptr<VkDescriptorBufferInfo> descriptorBufferInfoPtr;
    std::shared_ptr<VkBufferView> bufferViewHandlePtr;
    void *writeDescriptorSetNext;
  };

  struct CopyDescriptorSetParam {
    uint32_t srcDescriptorSetIndex;
    uint32_t srcBinding;
    uint32_t srcArrayElement;
    uint32_t dstDescriptorSetIndex;
    uint32_t dstBinding;
    uint32_t dstArrayElement;
    uint32_t descriptorCount;
  };

  DescriptorSetGroup(
      VkDevice &deviceHandleRef, VkDescriptorPool &descriptorPoolHandleRef,
      std::vector<VkDescriptorSetLayout> descriptorSetLayoutHandleList);

  ~DescriptorSetGroup();

  void updateDescriptorSets(
      std::vector<WriteDescriptorSetParam> writeDescriptorSetParamList,
      std::vector<CopyDescriptorSetParam> copyDescriptorSetParamList);

  void bindDescriptorSetsCmd(VkCommandBuffer &commandBufferHandleRef,
                             VkPipelineBindPoint pipelineBindPoint,
                             VkPipelineLayout &pipelineLayoutHandleRef,
                             uint32_t firstSet,
                             std::vector<uint32_t> descriptorSetIndexList,
                             std::vector<uint32_t> dynamicOffsetList);

private:
  std::vector<VkDescriptorSet> descriptorSetHandleList;

  VkDevice &deviceHandleRef;

  VkDescriptorPool &descriptorPoolHandleRef;
};
