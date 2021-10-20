#include "vrk/compute_pipeline_group.h"

ComputePipelineGroup::ComputePipelineGroup(VkDevice& deviceHandleRef,
    std::vector<ComputePipelineCreateInfoParam>
        computePipelineCreateInfoParamList) :
    deviceHandleRef(deviceHandleRef) {

  this->pipelineHandleList =
      std::vector<VkPipeline>(computePipelineCreateInfoParamList.size(),
      VK_NULL_HANDLE);

  std::vector<VkComputePipelineCreateInfo> computePipelineCreateInfoList = {};

  for (ComputePipelineCreateInfoParam& computePipelineCreateInfoParam :
      computePipelineCreateInfoParamList) {

    PipelineShaderStageCreateInfoParam pipelineShaderStageCreateInfoParam =
        computePipelineCreateInfoParam.pipelineShaderStageCreateInfoParam;

    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = NULL,
      .flags = 
          pipelineShaderStageCreateInfoParam.pipelineShaderStageCreateFlags,
      .stage = pipelineShaderStageCreateInfoParam.shaderStageFlagBits,
      .module = pipelineShaderStageCreateInfoParam.shaderModuleHandleRef,
      .pName = pipelineShaderStageCreateInfoParam.entryPointName.c_str(),
      .pSpecializationInfo = 
          pipelineShaderStageCreateInfoParam.specializationInfoPtr.get()
    };

    VkComputePipelineCreateInfo computePipelineCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
      .pNext = NULL,
      .flags = computePipelineCreateInfoParam.pipelineCreateFlags,
      .stage = pipelineShaderStageCreateInfo,
      .layout = computePipelineCreateInfoParam.pipelineLayoutHandleRef,
      .basePipelineHandle = computePipelineCreateInfoParam.basePipelineHandle,
      .basePipelineIndex = computePipelineCreateInfoParam.basePipelineIndex
    };
  }

  VkResult result = vkCreateComputePipelines(deviceHandleRef,
      VK_NULL_HANDLE,
      (uint32_t)computePipelineCreateInfoList.size(),
      computePipelineCreateInfoList.data(), NULL,
      this->pipelineHandleList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateComputePipelines");
  }
}

ComputePipelineGroup::~ComputePipelineGroup() {
  for (VkPipeline& pipeline : this->pipelineHandleList) {
    vkDestroyPipeline(this->deviceHandleRef, pipeline, NULL);
  }
}

void ComputePipelineGroup::bindPipelineCmd(uint32_t pipelineIndex,
    VkCommandBuffer& commandBufferHandleRef) {

  vkCmdBindPipeline(commandBufferHandleRef, VK_PIPELINE_BIND_POINT_COMPUTE,
      this->pipelineHandleList[pipelineIndex]);
}

void ComputePipelineGroup::dispatchCmd(VkCommandBuffer& commandBufferHandleRef,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ) {

  vkCmdDispatch(commandBufferHandleRef, groupCountX, groupCountY, groupCountZ);
}
