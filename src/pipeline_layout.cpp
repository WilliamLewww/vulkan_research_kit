#include "vrk/pipeline_layout.h"

PipelineLayout::PipelineLayout(
    VkDevice &deviceHandleRef,
    std::vector<VkDescriptorSetLayout> descriptorSetLayoutHandleList,
    std::vector<VkPushConstantRange> pushConstantRangeList)
    : deviceHandleRef(deviceHandleRef) {

  this->pipelineLayoutHandle = VK_NULL_HANDLE;

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .setLayoutCount = (uint32_t)descriptorSetLayoutHandleList.size(),
      .pSetLayouts = descriptorSetLayoutHandleList.data(),
      .pushConstantRangeCount = (uint32_t)pushConstantRangeList.size(),
      .pPushConstantRanges = pushConstantRangeList.data()};

  VkResult result =
      vkCreatePipelineLayout(deviceHandleRef, &pipelineLayoutCreateInfo, NULL,
                             &this->pipelineLayoutHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreatePipelineLayout");
  }
}

PipelineLayout::~PipelineLayout() {
  vkDestroyPipelineLayout(this->deviceHandleRef, this->pipelineLayoutHandle,
                          NULL);
}

VkPipelineLayout &PipelineLayout::getPipelineLayoutHandleRef() {
  return this->pipelineLayoutHandle;
}