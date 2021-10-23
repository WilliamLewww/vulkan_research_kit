#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class ComputePipelineGroup {
public:
  struct PipelineShaderStageCreateInfoParam {
    VkPipelineShaderStageCreateFlags pipelineShaderStageCreateFlags;
    VkShaderStageFlagBits shaderStageFlagBits;
    VkShaderModule &shaderModuleHandleRef;
    std::string entryPointName;
    std::shared_ptr<VkSpecializationInfo> specializationInfoPtr;
  };

  struct ComputePipelineCreateInfoParam {
    VkPipelineCreateFlags pipelineCreateFlags;
    PipelineShaderStageCreateInfoParam pipelineShaderStageCreateInfoParam;
    VkPipelineLayout &pipelineLayoutHandleRef;
    VkPipeline basePipelineHandle;
    int32_t basePipelineIndex;
  };

  ComputePipelineGroup(VkDevice &deviceHandleRef,
                       std::vector<ComputePipelineCreateInfoParam>
                           computePipelineCreateInfoParamList);

  ~ComputePipelineGroup();

  void bindPipelineCmd(uint32_t pipelineIndex,
                       VkCommandBuffer &commandBufferHandleRef);

  void dispatchCmd(VkCommandBuffer &commandBufferHandleRef,
                   uint32_t groupCountX, uint32_t groupCountY,
                   uint32_t groupCountZ);

private:
  std::vector<VkPipeline> pipelineHandleList;

  VkDevice &deviceHandleRef;
};