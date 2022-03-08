#include "vrk/descriptor_set_group.h"

DescriptorSetGroup::DescriptorSetGroup(
    VkDevice &deviceHandleRef, VkDescriptorPool &descriptorPoolHandleRef,
    std::vector<VkDescriptorSetLayout> descriptorSetLayoutHandleList)
    : deviceHandleRef(deviceHandleRef),
      descriptorPoolHandleRef(descriptorPoolHandleRef) {

  this->descriptorSetHandleList = std::vector<VkDescriptorSet>(
      descriptorSetLayoutHandleList.size(), VK_NULL_HANDLE);

  VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .pNext = NULL,
      .descriptorPool = descriptorPoolHandleRef,
      .descriptorSetCount = (uint32_t)descriptorSetLayoutHandleList.size(),
      .pSetLayouts = descriptorSetLayoutHandleList.data()};

  VkResult result =
      vkAllocateDescriptorSets(deviceHandleRef, &descriptorSetAllocateInfo,
                               this->descriptorSetHandleList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkAllocateDescriptorSets");
  }
}

DescriptorSetGroup::~DescriptorSetGroup() {
  VkResult result =
      vkFreeDescriptorSets(this->deviceHandleRef, this->descriptorPoolHandleRef,
                           this->descriptorSetHandleList.size(),
                           this->descriptorSetHandleList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkFreeDescriptorSets");
  }
}

void DescriptorSetGroup::updateDescriptorSets(
    std::vector<WriteDescriptorSetParam> writeDescriptorSetParamList,
    std::vector<CopyDescriptorSetParam> copyDescriptorSetParamList) {

  std::vector<VkWriteDescriptorSet> writeDescriptorSetList = {};

  for (WriteDescriptorSetParam &writeDescriptorSetParam :
       writeDescriptorSetParamList) {

    void *writeDescriptorSetNext = NULL;

    if (writeDescriptorSetParam.writeDescriptorSetNext != NULL) {
      writeDescriptorSetNext = writeDescriptorSetParam.writeDescriptorSetNext;
    }

    VkWriteDescriptorSet writeDescriptorSet = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = writeDescriptorSetNext,
        .dstSet = this->descriptorSetHandleList[writeDescriptorSetParam
                                                    .dstDescriptorSetIndex],
        .dstBinding = writeDescriptorSetParam.dstBinding,
        .dstArrayElement = writeDescriptorSetParam.dstArrayElement,
        .descriptorCount = writeDescriptorSetParam.descriptorCount,
        .descriptorType = writeDescriptorSetParam.descriptorType,
        .pImageInfo = writeDescriptorSetParam.descriptorImageInfoList.data(),
        .pBufferInfo = writeDescriptorSetParam.descriptorBufferInfoList.data(),
        .pTexelBufferView =
            writeDescriptorSetParam.bufferViewHandleList.data()};

    writeDescriptorSetList.push_back(writeDescriptorSet);
  }

  std::vector<VkCopyDescriptorSet> copyDescriptorSetList = {};

  for (CopyDescriptorSetParam &copyDescriptorSetParam :
       copyDescriptorSetParamList) {

    VkCopyDescriptorSet copyDescriptorSet = {
        .sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
        .pNext = NULL,
        .srcSet = this->descriptorSetHandleList[copyDescriptorSetParam
                                                    .srcDescriptorSetIndex],
        .srcBinding = copyDescriptorSetParam.srcBinding,
        .srcArrayElement = copyDescriptorSetParam.srcArrayElement,
        .dstSet = this->descriptorSetHandleList[copyDescriptorSetParam
                                                    .dstDescriptorSetIndex],
        .dstBinding = copyDescriptorSetParam.dstBinding,
        .dstArrayElement = copyDescriptorSetParam.dstArrayElement,
        .descriptorCount = copyDescriptorSetParam.descriptorCount};

    copyDescriptorSetList.push_back(copyDescriptorSet);
  }

  vkUpdateDescriptorSets(this->deviceHandleRef, writeDescriptorSetList.size(),
                         writeDescriptorSetList.data(),
                         copyDescriptorSetList.size(),
                         copyDescriptorSetList.data());
}

void DescriptorSetGroup::bindDescriptorSetsCmd(
    VkCommandBuffer &commandBufferHandleRef,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout &pipelineLayoutHandleRef, uint32_t firstSet,
    std::vector<uint32_t> descriptorSetIndexList,
    std::vector<uint32_t> dynamicOffsetList) {

  std::vector<VkDescriptorSet> descriptorSetHandleList = {};

  for (uint32_t descriptorSetIndex : descriptorSetIndexList) {
    descriptorSetHandleList.push_back(
        this->descriptorSetHandleList[descriptorSetIndex]);
  }

  vkCmdBindDescriptorSets(
      commandBufferHandleRef, pipelineBindPoint, pipelineLayoutHandleRef,
      firstSet, (uint32_t)descriptorSetHandleList.size(),
      descriptorSetHandleList.data(), (uint32_t)dynamicOffsetList.size(),
      dynamicOffsetList.data());
}
