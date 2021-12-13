#include "basic/material.h"
#include "basic/engine.h"
#include "basic/scene.h"

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

  this->descriptorPoolPtr = std::unique_ptr<DescriptorPool>(
      new DescriptorPool(enginePtr->devicePtr->getDeviceHandleRef(),
                         VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1,
                         {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 18}}));

  this->descriptorSetLayoutPtr = std::unique_ptr<DescriptorSetLayout>(
      new DescriptorSetLayout(enginePtr->devicePtr->getDeviceHandleRef(), 0,
                              {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
                                VK_SHADER_STAGE_VERTEX_BIT, NULL},
                               {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
                                VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
                               {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16,
                                VK_SHADER_STAGE_FRAGMENT_BIT, NULL}}));

  this->descriptorSetGroupPtr =
      std::unique_ptr<DescriptorSetGroup>(new DescriptorSetGroup(
          enginePtr->devicePtr->getDeviceHandleRef(),
          this->descriptorPoolPtr->getDescriptorPoolHandleRef(),
          {this->descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef()}));

  this->pipelineLayoutPtr = std::unique_ptr<PipelineLayout>(new PipelineLayout(
      enginePtr->devicePtr->getDeviceHandleRef(),
      {this->descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef()}, {}));

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
               offsetof(Model::Vertex, textureCoordinates)}}});

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
            this->pipelineLayoutPtr->getPipelineLayoutHandleRef(),
            enginePtr->renderPassPtr->getRenderPassHandleRef(),
            0,
            VK_NULL_HANDLE,
            0}}));
}

Material::~Material() {}

void Material::updateCameraDescriptorSet(std::shared_ptr<Camera> cameraPtr) {
  this->descriptorSetGroupPtr->updateDescriptorSets(
      {{0, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL,
        cameraPtr->getCameraDescriptorBufferInfoPtr(), NULL}},
      {});
}

void Material::updateSceneDescriptorSet(std::shared_ptr<Scene> scenePtr) {
  this->descriptorSetGroupPtr->updateDescriptorSets(
      {{0, 1, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL,
        scenePtr->getSceneDescriptorBufferInfoPtr(), NULL}},
      {});
}

void Material::updateEmptyLightDescriptors(std::shared_ptr<Buffer> bufferPtr) {
  for (uint32_t x = 0; x < 16; x++) {
    std::shared_ptr<VkDescriptorBufferInfo> lightDescriptorBufferInfoPtr =
        std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
            .buffer = bufferPtr->getBufferHandleRef(),
            .offset = x * sizeof(Light::LightShaderStructure),
            .range = sizeof(Light::LightShaderStructure)});

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{0, 2, x, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL,
          lightDescriptorBufferInfoPtr, NULL}},
        {});
  }
}

void Material::updateLightDescriptorSet(std::shared_ptr<Light> lightPtr) {
  this->descriptorSetGroupPtr->updateDescriptorSets(
      {{0, 2, lightPtr->getLightIndex(), 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        NULL, lightPtr->getLightDescriptorBufferInfoPtr(), NULL}},
      {});
}
