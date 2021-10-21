#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class RayTracingPipelineGroup {
public:
  struct PipelineShaderStageCreateInfoParam {
    VkPipelineShaderStageCreateFlags pipelineShaderStageCreateFlags;
    VkShaderStageFlagBits shaderStageFlagBits;
    VkShaderModule &shaderModuleHandleRef;
    std::string entryPointName;
    std::shared_ptr<VkSpecializationInfo> specializationInfoPtr;
  };

  struct RayTracingShaderGroupCreateInfoParam {
    VkRayTracingShaderGroupTypeKHR rayTracingShaderGroupType;
    uint32_t generalShader;
    uint32_t closestHitShader;
    uint32_t anyHitShader;
    uint32_t intersectionShader;
    std::shared_ptr<void> shaderGroupCaptureReplayHandlePtr;
  };

  struct PipelineLibraryCreateInfoParam {
    std::vector<VkPipeline> pipelineHandleList;
  };

  struct RayTracingPipelineInterfaceCreateInfoParam {
    uint32_t maxPipelineRayPayloadSize;
    uint32_t maxPipelineRayHitAttributeSize;
  };

  struct PipelineDynamicStateCreateInfoParam {
    VkPipelineDynamicStateCreateFlags pipelineDynamicStateCreateFlags;
    std::vector<VkDynamicState> dynamicStateList;
  };

  struct RayTracingPipelineCreateInfoParam {
    VkPipelineCreateFlags pipelineCreateFlags;
    std::vector<PipelineShaderStageCreateInfoParam>
        pipelineShaderStageCreateInfoParamList;
    std::vector<RayTracingShaderGroupCreateInfoParam>
        rayTracingShaderGroupCreateInfoParamList;
    uint32_t maxPipelineRayRecursionDepth;
    std::shared_ptr<PipelineLibraryCreateInfoParam>
        pipelineLibraryCreateInfoParamPtr;
    std::shared_ptr<RayTracingPipelineInterfaceCreateInfoParam>
        rayTracingPipelineInterfaceCreateInfoParamPtr;
    std::shared_ptr<PipelineDynamicStateCreateInfoParam>
        pipelineDynamicStateCreateInfoParamPtr;
    VkPipelineLayout &pipelineLayoutHandleRef;
    VkPipeline basePipelineHandle;
    int32_t basePipelineIndex;
  };

  RayTracingPipelineGroup(VkDevice &deviceHandleRef,
                          std::vector<RayTracingPipelineCreateInfoParam>
                              rayTracingPipelineCreateInfoParamList);

  ~RayTracingPipelineGroup();

  void bindPipelineCmd(uint32_t pipelineIndex,
                       VkCommandBuffer &commandBufferHandleRef);

  void traceRaysCmd(
      VkCommandBuffer &commandBufferHandleRef,
      std::shared_ptr<VkStridedDeviceAddressRegionKHR>
          raygenShaderBindingTablePtr,
      std::shared_ptr<VkStridedDeviceAddressRegionKHR>
          missShaderBindingTablePtr,
      std::shared_ptr<VkStridedDeviceAddressRegionKHR> hitShaderBindingTablePtr,
      std::shared_ptr<VkStridedDeviceAddressRegionKHR>
          callableShaderBindingTablePtr,
      uint32_t width, uint32_t height, uint32_t depth);

private:
  std::vector<VkPipeline> pipelineHandleList;

  VkDevice &deviceHandleRef;
};
