#include "vrk/graphics_pipeline_group.h"

GraphicsPipelineGroup::GraphicsPipelineGroup(
    VkDevice &deviceHandleRef, std::vector<GraphicsPipelineCreateInfoParam>
                                   graphicsPipelineCreateInfoParamList)
    : deviceHandleRef(deviceHandleRef) {

  this->pipelineHandleList = std::vector<VkPipeline>(
      graphicsPipelineCreateInfoParamList.size(), VK_NULL_HANDLE);

  std::vector<VkGraphicsPipelineCreateInfo> graphicsPipelineCreateInfoList = {};

  std::vector<std::vector<VkPipelineShaderStageCreateInfo>>
      pipelineShaderStageCreateInfoMainList = {};

  std::vector<VkPipelineVertexInputStateCreateInfo>
      pipelineVertexInputStateCreateInfoList = {};

  std::vector<VkPipelineInputAssemblyStateCreateInfo>
      pipelineInputAssemblyStateCreateInfoList = {};

  std::vector<VkPipelineTessellationStateCreateInfo>
      pipelineTessellationStateCreateInfoList = {};

  std::vector<VkPipelineViewportStateCreateInfo>
      pipelineViewportStateCreateInfoList = {};

  std::vector<VkPipelineRasterizationStateCreateInfo>
      pipelineRasterizationStateCreateInfoList = {};

  std::vector<VkPipelineMultisampleStateCreateInfo>
      pipelineMultisampleStateCreateInfoList = {};

  std::vector<VkPipelineDepthStencilStateCreateInfo>
      pipelineDepthStencilStateCreateInfoList = {};

  std::vector<VkPipelineColorBlendStateCreateInfo>
      pipelineColorBlendStateCreateInfoList = {};

  std::vector<VkPipelineDynamicStateCreateInfo>
      pipelineDynamicStateCreateInfoList = {};

  for (GraphicsPipelineCreateInfoParam &graphicsPipelineCreateInfoParam :
       graphicsPipelineCreateInfoParamList) {

    std::vector<VkPipelineShaderStageCreateInfo>
        pipelineShaderStageCreateInfoList = {};

    for (PipelineShaderStageCreateInfoParam
             &pipelineShaderStageCreateInfoParam :
         graphicsPipelineCreateInfoParam
             .pipelineShaderStageCreateInfoParamList) {

      VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
          .pNext = NULL,
          .flags =
              pipelineShaderStageCreateInfoParam.pipelineShaderStageCreateFlags,
          .stage = pipelineShaderStageCreateInfoParam.shaderStageFlagBits,
          .module = pipelineShaderStageCreateInfoParam.shaderModuleHandleRef,
          .pName = pipelineShaderStageCreateInfoParam.entryPointName.c_str(),
          .pSpecializationInfo =
              pipelineShaderStageCreateInfoParam.specializationInfoPtr.get()};

      pipelineShaderStageCreateInfoList.push_back(
          pipelineShaderStageCreateInfo);
    }

    pipelineShaderStageCreateInfoMainList.push_back(
        pipelineShaderStageCreateInfoList);

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = graphicsPipelineCreateInfoParam.pipelineCreateFlags,
        .stageCount = (uint32_t)pipelineShaderStageCreateInfoMainList
                          [pipelineShaderStageCreateInfoMainList.size() - 1]
                              .size(),
        .pStages = pipelineShaderStageCreateInfoMainList
                       [pipelineShaderStageCreateInfoMainList.size() - 1]
                           .data(),
        .pVertexInputState = NULL,
        .pInputAssemblyState = NULL,
        .pTessellationState = NULL,
        .pViewportState = NULL,
        .pRasterizationState = NULL,
        .pMultisampleState = NULL,
        .pDepthStencilState = NULL,
        .pColorBlendState = NULL,
        .pDynamicState = NULL,
        .layout = graphicsPipelineCreateInfoParam.pipelineLayoutHandleRef,
        .renderPass = graphicsPipelineCreateInfoParam.renderPassHandleRef,
        .subpass = graphicsPipelineCreateInfoParam.subpass,
        .basePipelineHandle =
            graphicsPipelineCreateInfoParam.basePipelineHandle,
        .basePipelineIndex = graphicsPipelineCreateInfoParam.basePipelineIndex};

    if (graphicsPipelineCreateInfoParam
            .pipelineVertexInputStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineVertexInputStateCreateInfoParam>
          pipelineVertexInputStateCreateInfoParamPtr =
              graphicsPipelineCreateInfoParam
                  .pipelineVertexInputStateCreateInfoParamPtr;

      VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo =
          {
              .sType =
                  VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
              .pNext = NULL,
              .flags = 0,
              .vertexBindingDescriptionCount =
                  (uint32_t)pipelineVertexInputStateCreateInfoParamPtr
                      ->vertexInputBindingDescriptionList.size(),
              .pVertexBindingDescriptions =
                  pipelineVertexInputStateCreateInfoParamPtr
                      ->vertexInputBindingDescriptionList.data(),
              .vertexAttributeDescriptionCount =
                  (uint32_t)pipelineVertexInputStateCreateInfoParamPtr
                      ->vertexInputAttributeDescriptionList.size(),
              .pVertexAttributeDescriptions =
                  pipelineVertexInputStateCreateInfoParamPtr
                      ->vertexInputAttributeDescriptionList.data()};

      pipelineVertexInputStateCreateInfoList.push_back(
          pipelineVertexInputStateCreateInfo);

      graphicsPipelineCreateInfo.pVertexInputState =
          &pipelineVertexInputStateCreateInfoList
              [pipelineVertexInputStateCreateInfoList.size() - 1];
    }

    if (graphicsPipelineCreateInfoParam
            .pipelineInputAssemblyStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineInputAssemblyStateCreateInfoParam>
          pipelineInputAssemblyStateCreateInfoParamPtr =
              graphicsPipelineCreateInfoParam
                  .pipelineInputAssemblyStateCreateInfoParamPtr;

      VkPipelineInputAssemblyStateCreateInfo
          pipelineInputAssemblyStateCreateInfo = {
              .sType =
                  VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
              .pNext = NULL,
              .flags = 0,
              .topology = pipelineInputAssemblyStateCreateInfoParamPtr
                              ->primitiveTopology,
              .primitiveRestartEnable =
                  pipelineInputAssemblyStateCreateInfoParamPtr
                      ->primitiveRestartEnable};

      pipelineInputAssemblyStateCreateInfoList.push_back(
          pipelineInputAssemblyStateCreateInfo);

      graphicsPipelineCreateInfo.pInputAssemblyState =
          &pipelineInputAssemblyStateCreateInfoList
              [pipelineInputAssemblyStateCreateInfoList.size() - 1];
    }

    if (graphicsPipelineCreateInfoParam
            .pipelineTessellationStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineTessellationStateCreateInfoParam>
          pipelineTessellationStateCreateInfoParamPtr =
              graphicsPipelineCreateInfoParam
                  .pipelineTessellationStateCreateInfoParamPtr;

      VkPipelineTessellationStateCreateInfo
          pipelineTessellationStateCreateInfo = {
              .sType =
                  VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
              .pNext = NULL,
              .flags = 0,
              .patchControlPoints = pipelineTessellationStateCreateInfoParamPtr
                                        ->patchControlPoints};

      pipelineTessellationStateCreateInfoList.push_back(
          pipelineTessellationStateCreateInfo);

      graphicsPipelineCreateInfo.pTessellationState =
          &pipelineTessellationStateCreateInfoList
              [pipelineTessellationStateCreateInfoList.size() - 1];
    }

    if (graphicsPipelineCreateInfoParam
            .pipelineViewportStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineViewportStateCreateInfoParam>
          pipelineViewportStateCreateInfoParamPtr =
              graphicsPipelineCreateInfoParam
                  .pipelineViewportStateCreateInfoParamPtr;

      VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .viewportCount = (uint32_t)pipelineViewportStateCreateInfoParamPtr
                               ->viewportList.size(),
          .pViewports =
              pipelineViewportStateCreateInfoParamPtr->viewportList.data(),
          .scissorCount = (uint32_t)pipelineViewportStateCreateInfoParamPtr
                              ->scissorRect2DList.size(),
          .pScissors =
              pipelineViewportStateCreateInfoParamPtr->scissorRect2DList.data(),
      };

      pipelineViewportStateCreateInfoList.push_back(
          pipelineViewportStateCreateInfo);

      graphicsPipelineCreateInfo.pViewportState =
          &pipelineViewportStateCreateInfoList
              [pipelineViewportStateCreateInfoList.size() - 1];
    }

    if (graphicsPipelineCreateInfoParam
            .pipelineRasterizationStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineRasterizationStateCreateInfoParam>
          pipelineRasterizationStateCreateInfoParamPtr =
              graphicsPipelineCreateInfoParam
                  .pipelineRasterizationStateCreateInfoParamPtr;

      VkPipelineRasterizationStateCreateInfo
          pipelineRasterizationStateCreateInfo = {
              .sType =
                  VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
              .pNext = NULL,
              .flags = 0,
              .depthClampEnable = pipelineRasterizationStateCreateInfoParamPtr
                                      ->depthClampEnable,
              .rasterizerDiscardEnable =
                  pipelineRasterizationStateCreateInfoParamPtr
                      ->rasterizerDiscardEnable,
              .polygonMode =
                  pipelineRasterizationStateCreateInfoParamPtr->polygonMode,
              .cullMode =
                  pipelineRasterizationStateCreateInfoParamPtr->cullModeFlags,
              .frontFace =
                  pipelineRasterizationStateCreateInfoParamPtr->frontFace,
              .depthBiasEnable =
                  pipelineRasterizationStateCreateInfoParamPtr->depthBiasEnable,
              .depthBiasConstantFactor =
                  pipelineRasterizationStateCreateInfoParamPtr
                      ->depthBiasConstantFactor,
              .depthBiasClamp =
                  pipelineRasterizationStateCreateInfoParamPtr->depthBiasClamp,
              .depthBiasSlopeFactor =
                  pipelineRasterizationStateCreateInfoParamPtr
                      ->depthBiasSlopeFactor,
              .lineWidth =
                  pipelineRasterizationStateCreateInfoParamPtr->lineWidth};

      pipelineRasterizationStateCreateInfoList.push_back(
          pipelineRasterizationStateCreateInfo);

      graphicsPipelineCreateInfo.pRasterizationState =
          &pipelineRasterizationStateCreateInfoList
              [pipelineRasterizationStateCreateInfoList.size() - 1];
    }

    if (graphicsPipelineCreateInfoParam
            .pipelineMultisampleStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineMultisampleStateCreateInfoParam>
          pipelineMultisampleStateCreateInfoParamPtr =
              graphicsPipelineCreateInfoParam
                  .pipelineMultisampleStateCreateInfoParamPtr;

      VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo =
          {
              .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
              .pNext = NULL,
              .flags = 0,
              .rasterizationSamples = pipelineMultisampleStateCreateInfoParamPtr
                                          ->rasterizationSampleCountFlagBits,
              .sampleShadingEnable = pipelineMultisampleStateCreateInfoParamPtr
                                         ->sampleShadingEnable,
              .minSampleShading =
                  pipelineMultisampleStateCreateInfoParamPtr->minSampleShading,
              .pSampleMask = pipelineMultisampleStateCreateInfoParamPtr
                                 ->sampleMaskList.data(),
              .alphaToCoverageEnable =
                  pipelineMultisampleStateCreateInfoParamPtr
                      ->alphaToCoverageEnable,
              .alphaToOneEnable =
                  pipelineMultisampleStateCreateInfoParamPtr->alphaToOneEnable};

      pipelineMultisampleStateCreateInfoList.push_back(
          pipelineMultisampleStateCreateInfo);

      graphicsPipelineCreateInfo.pMultisampleState =
          &pipelineMultisampleStateCreateInfoList
              [pipelineMultisampleStateCreateInfoList.size() - 1];
    }

    if (graphicsPipelineCreateInfoParam
            .pipelineDepthStencilStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineDepthStencilStateCreateInfoParam>
          pipelineDepthStencilStateCreateInfoParamPtr =
              graphicsPipelineCreateInfoParam
                  .pipelineDepthStencilStateCreateInfoParamPtr;

      VkPipelineDepthStencilStateCreateInfo
          pipelineDepthStencilStateCreateInfo = {
              .sType =
                  VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
              .pNext = NULL,
              .flags = 0,
              .depthTestEnable =
                  pipelineDepthStencilStateCreateInfoParamPtr->depthTestEnable,
              .depthWriteEnable =
                  pipelineDepthStencilStateCreateInfoParamPtr->depthWriteEnable,
              .depthCompareOp =
                  pipelineDepthStencilStateCreateInfoParamPtr->depthCompareOp,
              .depthBoundsTestEnable =
                  pipelineDepthStencilStateCreateInfoParamPtr
                      ->depthBoundsTestEnable,
              .stencilTestEnable = pipelineDepthStencilStateCreateInfoParamPtr
                                       ->stencilTestEnable,
              .front = pipelineDepthStencilStateCreateInfoParamPtr
                           ->frontStencilOpState,
              .back = pipelineDepthStencilStateCreateInfoParamPtr
                          ->backStencilOpState,
              .minDepthBounds =
                  pipelineDepthStencilStateCreateInfoParamPtr->minDepthBounds,
              .maxDepthBounds =
                  pipelineDepthStencilStateCreateInfoParamPtr->maxDepthBounds};

      pipelineDepthStencilStateCreateInfoList.push_back(
          pipelineDepthStencilStateCreateInfo);

      graphicsPipelineCreateInfo.pDepthStencilState =
          &pipelineDepthStencilStateCreateInfoList
              [pipelineDepthStencilStateCreateInfoList.size() - 1];
    }

    if (graphicsPipelineCreateInfoParam
            .pipelineColorBlendStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineColorBlendStateCreateInfoParam>
          pipelineColorBlendStateCreateInfoParamPtr =
              graphicsPipelineCreateInfoParam
                  .pipelineColorBlendStateCreateInfoParamPtr;

      VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .logicOpEnable =
              pipelineColorBlendStateCreateInfoParamPtr->logicOpEnable,
          .logicOp = pipelineColorBlendStateCreateInfoParamPtr->logicOp,
          .attachmentCount = (uint32_t)pipelineColorBlendStateCreateInfoParamPtr
                                 ->pipelineColorBlendAttachmentStateList.size(),
          .pAttachments = pipelineColorBlendStateCreateInfoParamPtr
                              ->pipelineColorBlendAttachmentStateList.data(),
          .blendConstants = {
              pipelineColorBlendStateCreateInfoParamPtr->blendConstants[0],
              pipelineColorBlendStateCreateInfoParamPtr->blendConstants[1],
              pipelineColorBlendStateCreateInfoParamPtr->blendConstants[2],
              pipelineColorBlendStateCreateInfoParamPtr->blendConstants[3]}};

      pipelineColorBlendStateCreateInfoList.push_back(
          pipelineColorBlendStateCreateInfo);

      graphicsPipelineCreateInfo.pColorBlendState =
          &pipelineColorBlendStateCreateInfoList
              [pipelineColorBlendStateCreateInfoList.size() - 1];
    }

    if (graphicsPipelineCreateInfoParam
            .pipelineDynamicStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineDynamicStateCreateInfoParam>
          pipelineDynamicStateCreateInfoParamPtr =
              graphicsPipelineCreateInfoParam
                  .pipelineDynamicStateCreateInfoParamPtr;

      VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {

          .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .dynamicStateCount = (uint32_t)pipelineDynamicStateCreateInfoParamPtr
                                   ->dynamicStateList.size(),
          .pDynamicStates =
              pipelineDynamicStateCreateInfoParamPtr->dynamicStateList.data()};

      pipelineDynamicStateCreateInfoList.push_back(
          pipelineDynamicStateCreateInfo);

      graphicsPipelineCreateInfo.pDynamicState =
          &pipelineDynamicStateCreateInfoList
              [pipelineDynamicStateCreateInfoList.size() - 1];
    }

    graphicsPipelineCreateInfoList.push_back(graphicsPipelineCreateInfo);
  }

  VkResult result =
      vkCreateGraphicsPipelines(deviceHandleRef, VK_NULL_HANDLE,
                                (uint32_t)graphicsPipelineCreateInfoList.size(),
                                graphicsPipelineCreateInfoList.data(), NULL,
                                this->pipelineHandleList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateGraphicsPipelines");
  }
}

GraphicsPipelineGroup::~GraphicsPipelineGroup() {
  for (VkPipeline &pipeline : this->pipelineHandleList) {
    vkDestroyPipeline(this->deviceHandleRef, pipeline, NULL);
  }
}

void GraphicsPipelineGroup::bindPipelineCmd(
    uint32_t pipelineIndex, VkCommandBuffer &commandBufferHandleRef) {

  vkCmdBindPipeline(commandBufferHandleRef, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    this->pipelineHandleList[pipelineIndex]);
}

void GraphicsPipelineGroup::drawIndexedCmd(
    VkCommandBuffer &commandBufferHandleRef, uint32_t indexCount,
    uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset,
    uint32_t firstInstance) {

  vkCmdDrawIndexed(commandBufferHandleRef, indexCount, instanceCount,
                   firstIndex, vertexOffset, firstInstance);
}
