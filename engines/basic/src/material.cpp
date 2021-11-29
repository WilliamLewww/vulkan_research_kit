#include "basic/material.h"
#include "basic/engine.h"

Material::Material(std::shared_ptr<Engine> enginePtr, std::string materialName,
                   std::string vertexFileName, std::string fragmentFileName)
    : enginePtr(enginePtr), materialName(materialName) {

  std::ifstream vertexFile(Resource::findResource(vertexFileName),
                           std::ios::binary | std::ios::ate);
  std::streamsize vertexFileSize = vertexFile.tellg();
  vertexFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> vertexShaderSource(vertexFileSize / sizeof(uint32_t));
  vertexFile.read((char *)vertexShaderSource.data(), vertexFileSize);
  vertexFile.close();

  this->vertexShaderModulePtr = std::unique_ptr<ShaderModule>(new ShaderModule(
      enginePtr->devicePtr->getDeviceHandleRef(), vertexShaderSource));

  std::ifstream fragmentFile(Resource::findResource(fragmentFileName),
                             std::ios::binary | std::ios::ate);
  std::streamsize fragmentFileSize = fragmentFile.tellg();
  fragmentFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> fragmentShaderSource(fragmentFileSize /
                                             sizeof(uint32_t));
  fragmentFile.read((char *)fragmentShaderSource.data(), fragmentFileSize);
  fragmentFile.close();

  this->fragmentShaderModulePtr =
      std::unique_ptr<ShaderModule>(new ShaderModule(
          enginePtr->devicePtr->getDeviceHandleRef(), fragmentShaderSource));

  GraphicsPipelineGroup::PipelineShaderStageCreateInfoParam vertexStage = {
      .pipelineShaderStageCreateFlags = 0,
      .shaderStageFlagBits = VK_SHADER_STAGE_VERTEX_BIT,
      .shaderModuleHandleRef =
          this->vertexShaderModulePtr->getShaderModuleHandleRef(),
      .entryPointName = "main",
      .specializationInfoPtr = NULL};

  GraphicsPipelineGroup::PipelineShaderStageCreateInfoParam fragmentStage = {
      .pipelineShaderStageCreateFlags = 0,
      .shaderStageFlagBits = VK_SHADER_STAGE_FRAGMENT_BIT,
      .shaderModuleHandleRef =
          this->fragmentShaderModulePtr->getShaderModuleHandleRef(),
      .entryPointName = "main",
      .specializationInfoPtr = NULL};

  PipelineLayout *pipelineLayout = new PipelineLayout(
      enginePtr->devicePtr->getDeviceHandleRef(),
      std::vector<VkDescriptorSetLayout>(), std::vector<VkPushConstantRange>());

  auto pipelineVertexInputStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineVertexInputStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineVertexInputStateCreateInfoParam{

          .vertexInputBindingDescriptionList = {{0, sizeof(float) * 3,
                                                 VK_VERTEX_INPUT_RATE_VERTEX}},
          .vertexInputAttributeDescriptionList = {
              {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0}}});

  auto pipelineInputAssemblyStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineInputAssemblyStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineInputAssemblyStateCreateInfoParam{

          .primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
          // .primitiveRestartEnable =
      });

  auto pipelineViewportStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineViewportStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineViewportStateCreateInfoParam{

          .viewportList = {{0, 0, 800, 600, 0, 1}},
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

  this->graphicsPipelineGroupPtr =
      std::unique_ptr<GraphicsPipelineGroup>(new GraphicsPipelineGroup(
          enginePtr->devicePtr->getDeviceHandleRef(),
          {{0,
            {vertexStage, fragmentStage},
            pipelineVertexInputStateCreateInfoParam,
            pipelineInputAssemblyStateCreateInfoParam,
            NULL,
            pipelineViewportStateCreateInfoParam,
            pipelineRasterizationStateCreateInfoParam,
            pipelineMultisampleStateCreateInfoParam,
            NULL,
            pipelineColorBlendStateCreateInfoParam,
            NULL,
            pipelineLayout->getPipelineLayoutHandleRef(),
            enginePtr->renderPassPtr->getRenderPassHandleRef(),
            0,
            VK_NULL_HANDLE,
            0}}));
}

Material::~Material() {}
