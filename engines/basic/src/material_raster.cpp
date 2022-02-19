#include "basic/material_raster.h"
#include "basic/engine.h"
#include "basic/scene.h"

MaterialRaster::MaterialRaster(
    std::shared_ptr<Engine> enginePtr, std::string materialName,
    std::map<ShaderStage, std::string> shaderStageNameMap)
    : Material(enginePtr, materialName, shaderStageNameMap,
               Material::MaterialType::RASTER) {

  std::vector<GraphicsPipelineGroup::PipelineShaderStageCreateInfoParam>
      pipelineShaderStageCreateInfoList;

  for (auto &pair : this->shaderStageModuleMap) {
    GraphicsPipelineGroup::PipelineShaderStageCreateInfoParam shaderStage = {
        .pipelineShaderStageCreateFlags = 0,
        .shaderStageFlagBits = (VkShaderStageFlagBits)0,
        .shaderModuleHandleRef = pair.second->getShaderModuleHandleRef(),
        .entryPointName = "main",
        .specializationInfoPtr = NULL};

    if (pair.first == ShaderStage::VERTEX) {
      shaderStage.shaderStageFlagBits = VK_SHADER_STAGE_VERTEX_BIT;
    } else if (pair.first == ShaderStage::FRAGMENT) {
      shaderStage.shaderStageFlagBits = VK_SHADER_STAGE_FRAGMENT_BIT;
    } else if (pair.first == ShaderStage::GEOMETRY) {
      shaderStage.shaderStageFlagBits = VK_SHADER_STAGE_GEOMETRY_BIT;
    }

    pipelineShaderStageCreateInfoList.push_back(shaderStage);
  }

  auto pipelineVertexInputStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineVertexInputStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineVertexInputStateCreateInfoParam{

          .vertexInputBindingDescriptionList = {{0, sizeof(Model::Vertex),
                                                 VK_VERTEX_INPUT_RATE_VERTEX}},
          .vertexInputAttributeDescriptionList = {
              {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
              {1, 0, VK_FORMAT_R32G32B32_SFLOAT,
               offsetof(Model::Vertex, normals)},
              {2, 0, VK_FORMAT_R32G32_SFLOAT,
               offsetof(Model::Vertex, textureCoordinates)},
              {3, 0, VK_FORMAT_R32_SINT,
               offsetof(Model::Vertex, materialPropertiesIndex)},
              {4, 0, VK_FORMAT_R32_SINT,
               offsetof(Model::Vertex, modelIndex)}}});

  auto pipelineInputAssemblyStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineInputAssemblyStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineInputAssemblyStateCreateInfoParam{

          .primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
          // .primitiveRestartEnable =
      });

  auto pipelineViewportStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineViewportStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineViewportStateCreateInfoParam{

          .viewportList = {{0, 600, 800, -600, 0, 1}},
          .scissorRect2DList = {{{0, 0}, {800, 600}}}});

  auto pipelineRasterizationStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineRasterizationStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineRasterizationStateCreateInfoParam{

          // .depthClampEnable = ,
          // .rasterizerDiscardEnable = ,
          // .polygonMode = ,
          // .cullModeFlags = ,
          // .frontFace = ,
          // .depthBiasEnable = ,
          // .depthBiasConstantFactor = ,
          // .depthBiasClamp = ,
          // .depthBiasSlopeFactor = ,
          .lineWidth = 1.0});

  auto pipelineMultisampleStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineMultisampleStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineMultisampleStateCreateInfoParam{

          .rasterizationSampleCountFlagBits = VK_SAMPLE_COUNT_1_BIT,
          // .sampleShadingEnable = ,
          // .minSampleShading = ,
          // .sampleMaskList = ,
          // .alphaToCoverageEnable = ,
          // .alphaToOneEnable =
      });

  auto pipelineDepthStencilStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineDepthStencilStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineDepthStencilStateCreateInfoParam{

          .depthTestEnable = VK_TRUE,
          .depthWriteEnable = VK_TRUE,
          .depthCompareOp = VK_COMPARE_OP_LESS,
          .depthBoundsTestEnable = VK_FALSE,
          .stencilTestEnable = VK_FALSE,
          // .frontStencilOpState = ,
          // .backStencilOpState = ,
          // .minDepthBounds = ,
          // .maxDepthBounds =
      });

  VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {
      .blendEnable = VK_FALSE,
      .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

  auto pipelineColorBlendStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineColorBlendStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineColorBlendStateCreateInfoParam{

          .logicOpEnable = VK_FALSE,
          .logicOp = VK_LOGIC_OP_COPY,
          .pipelineColorBlendAttachmentStateList =
              {pipelineColorBlendAttachmentState},
          .blendConstants = {0, 0, 0, 0}});

  this->rasterDescriptorSetLayoutPtr =
      std::shared_ptr<DescriptorSetLayout>(new DescriptorSetLayout(
          enginePtr->getDevicePtr()->getDeviceHandleRef(), 0, {}));

  this->initializeDescriptors(this->RASTER_MATERIAL_DESCRIPTOR_COUNTS,
                              this->rasterDescriptorSetLayoutPtr);

  this->graphicsPipelineGroupPtr =
      std::unique_ptr<GraphicsPipelineGroup>(new GraphicsPipelineGroup(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          {{0, pipelineShaderStageCreateInfoList,
            pipelineVertexInputStateCreateInfoParam,
            pipelineInputAssemblyStateCreateInfoParam, NULL,
            pipelineViewportStateCreateInfoParam,
            pipelineRasterizationStateCreateInfoParam,
            pipelineMultisampleStateCreateInfoParam,
            pipelineDepthStencilStateCreateInfoParam,
            pipelineColorBlendStateCreateInfoParam, NULL,
            this->pipelineLayoutPtr->getPipelineLayoutHandleRef(),
            enginePtr->getRenderPassPtr()->getRenderPassHandleRef(), 0,
            VK_NULL_HANDLE, 0}}));
}

MaterialRaster::~MaterialRaster() {}

void MaterialRaster::render(VkCommandBuffer commandBufferHandle,
                            std::shared_ptr<Model> modelPtr) {

  this->graphicsPipelineGroupPtr->bindPipelineCmd(0, commandBufferHandle);

  modelPtr->getVertexBufferPtr()->bindVertexBufferCmd(commandBufferHandle, 0);

  modelPtr->getIndexBufferPtr()->bindIndexBufferCmd(commandBufferHandle,
                                                    VK_INDEX_TYPE_UINT32);

  this->descriptorSetGroupPtr->bindDescriptorSetsCmd(
      commandBufferHandle, VK_PIPELINE_BIND_POINT_GRAPHICS,
      this->pipelineLayoutPtr->getPipelineLayoutHandleRef(), 0, {0, 1}, {});

  this->graphicsPipelineGroupPtr->drawIndexedCmd(
      commandBufferHandle, modelPtr->getIndexCount(), 1, 0, 0, 0);
}
