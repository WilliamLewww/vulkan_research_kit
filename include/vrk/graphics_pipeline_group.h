#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

class GraphicsPipelineGroup {
public:
  struct PipelineShaderStageCreateInfoParam {
    VkPipelineShaderStageCreateFlags pipelineShaderStageCreateFlags;
    VkShaderStageFlagBits shaderStageFlagBits;
    VkShaderModule& shaderModuleHandleRef;
    std::string entryPointName;
    std::shared_ptr<VkSpecializationInfo> specializationInfoPtr;
  };

  struct PipelineVertexInputStateCreateInfoParam {
    std::vector<VkVertexInputBindingDescription>
        vertexInputBindingDescriptionList;
    std::vector<VkVertexInputAttributeDescription>
        vertexInputAttributeDescriptionList;
  };

  struct PipelineInputAssemblyStateCreateInfoParam {
    VkPrimitiveTopology primitiveTopology;
    VkBool32 primitiveRestartEnable;
  };

  struct PipelineTessellationStateCreateInfoParam {
    uint32_t patchControlPoints;
  };

  struct PipelineViewportStateCreateInfoParam {
    std::vector<VkViewport> viewportList;
    std::vector<VkRect2D> scissorRect2DList;
  };

  struct PipelineRasterizationStateCreateInfoParam {
    VkBool32 depthClampEnable;
    VkBool32 rasterizerDiscardEnable;
    VkPolygonMode polygonMode;
    VkCullModeFlags cullModeFlags;
    VkFrontFace frontFace;
    VkBool32 depthBiasEnable;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    float lineWidth;
  };

  struct PipelineMultisampleStateCreateInfoParam {
    VkSampleCountFlagBits rasterizationSampleCountFlagBits;
    VkBool32 sampleShadingEnable;
    float minSampleShading;
    std::vector<VkSampleMask> sampleMaskList;
    VkBool32 alphaToCoverageEnable;
    VkBool32 alphaToOneEnable;
  };

  struct PipelineDepthStencilStateCreateInfoParam {
    VkBool32 depthTestEnable;
    VkBool32 depthWriteEnable;
    VkCompareOp depthCompareOp;
    VkBool32 depthBoundsTestEnable;
    VkBool32 stencilTestEnable;
    VkStencilOpState frontStencilOpState;
    VkStencilOpState backStencilOpState;
    float minDepthBounds;
    float maxDepthBounds;
  };

  struct PipelineColorBlendStateCreateInfoParam {
    VkBool32 logicOpEnable;
    VkLogicOp logicOp;
    std::vector<VkPipelineColorBlendAttachmentState>
        pipelineColorBlendAttachmentStateList;
    float blendConstants[4];
  };

  struct PipelineDynamicStateCreateInfoParam {
    std::vector<VkDynamicState> dynamicStateList;
  };

  struct GraphicsPipelineCreateInfoParam {
    VkPipelineCreateFlags pipelineCreateFlags;
    std::vector<PipelineShaderStageCreateInfoParam>
        pipelineShaderStageCreateInfoParamList;

    std::shared_ptr<PipelineVertexInputStateCreateInfoParam>
        pipelineVertexInputStateCreateInfoParamPtr;

    std::shared_ptr<PipelineInputAssemblyStateCreateInfoParam>
        pipelineInputAssemblyStateCreateInfoParamPtr;

    std::shared_ptr<PipelineTessellationStateCreateInfoParam> 
        pipelineTessellationStateCreateInfoParamPtr;

    std::shared_ptr<PipelineViewportStateCreateInfoParam> 
        pipelineViewportStateCreateInfoParamPtr;

    std::shared_ptr<PipelineRasterizationStateCreateInfoParam> 
        pipelineRasterizationStateCreateInfoParamPtr;

    std::shared_ptr<PipelineMultisampleStateCreateInfoParam> 
        pipelineMultisampleStateCreateInfoParamPtr;

    std::shared_ptr<PipelineDepthStencilStateCreateInfoParam> 
        pipelineDepthStencilStateCreateInfoParamPtr;

    std::shared_ptr<PipelineColorBlendStateCreateInfoParam> 
        pipelineColorBlendStateCreateInfoParamPtr;

    std::shared_ptr<PipelineDynamicStateCreateInfoParam> 
        pipelineDynamicStateCreateInfoParamPtr;

    VkPipelineLayout& pipelineLayoutHandleRef;
    VkRenderPass& renderPassHandleRef;
    uint32_t subpass;
    VkPipeline basePipelineHandle;
    int32_t basePipelineIndex;
  };

  GraphicsPipelineGroup(VkDevice& deviceHandleRef,
      std::vector<GraphicsPipelineCreateInfoParam>
          graphicsPipelineCreateInfoParamList);

  ~GraphicsPipelineGroup();
private:
  std::vector<VkPipeline> pipelineHandleList;

  VkDevice& deviceHandleRef;
};