#include "basic/material_raster.h"
#include "basic/engine.h"
#include "basic/scene.h"

MaterialRaster::MaterialRaster(
    std::shared_ptr<Engine> enginePtr, std::string materialName,
    std::map<ShaderStage, std::string> shaderStageNameMap)
    : Material(enginePtr, materialName, shaderStageNameMap,
               Material::MaterialType::RASTER) {

  std::vector<VkAttachmentReference> attachmentReferenceList = {
      {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
      {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};

  this->renderPassPtr = std::unique_ptr<RenderPass>(new RenderPass(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      (VkRenderPassCreateFlagBits)0,
      {{0, enginePtr->getSurfaceFormatList()[0].format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
       {0, VK_FORMAT_D32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}},
      {{
          0,
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          0,
          NULL,
          1,
          &attachmentReferenceList[0],
          NULL,
          &attachmentReferenceList[1],
          0,
          NULL,
      }},
      {}));

  for (uint32_t x = 0; x < enginePtr->getSwapchainImageCount(); x++) {
    this->imagePtrMap["output" + x] = std::unique_ptr<Image>(new Image(
        enginePtr->getDevicePtr()->getDeviceHandleRef(),
        *enginePtr->getPhysicalDeviceHandlePtr().get(), 0, VK_IMAGE_TYPE_2D,
        enginePtr->getSurfaceFormatList()[0].format,
        {enginePtr->getSurfaceCapabilities().currentExtent.width,
         enginePtr->getSurfaceCapabilities().currentExtent.height, 1},
        1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
        VK_IMAGE_LAYOUT_UNDEFINED, 0));

    this->imageViewPtrMap["output" + x] =
        std::unique_ptr<ImageView>(new ImageView(
            enginePtr->getDevicePtr()->getDeviceHandleRef(),
            this->imagePtrMap["output" + x]->getImageHandleRef(), 0,
            VK_IMAGE_VIEW_TYPE_2D, enginePtr->getSurfaceFormatList()[0].format,
            {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
             VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}));

    this->imagePtrMap["depth" + x] = std::unique_ptr<Image>(new Image(
        enginePtr->getDevicePtr()->getDeviceHandleRef(),
        *enginePtr->getPhysicalDeviceHandlePtr().get(), 0, VK_IMAGE_TYPE_2D,
        VK_FORMAT_D32_SFLOAT,
        {enginePtr->getSurfaceCapabilities().currentExtent.width,
         enginePtr->getSurfaceCapabilities().currentExtent.height, 1},
        1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE,
        {enginePtr->getQueueFamilyIndex()}, VK_IMAGE_LAYOUT_UNDEFINED, 0));

    this->imageViewPtrMap["depth" + x] =
        std::unique_ptr<ImageView>(new ImageView(
            enginePtr->getDevicePtr()->getDeviceHandleRef(),
            this->imagePtrMap["depth" + x]->getImageHandleRef(), 0,
            VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_D32_SFLOAT,
            {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
             VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1}));

    this->framebufferPtrList.push_back(
        std::shared_ptr<Framebuffer>(new Framebuffer(
            enginePtr->getDevicePtr()->getDeviceHandleRef(),
            this->renderPassPtr->getRenderPassHandleRef(),
            {this->imageViewPtrMap["output" + x]->getImageViewHandleRef(),
             this->imageViewPtrMap["depth" + x]->getImageViewHandleRef()},
            (VkFramebufferCreateFlags)0,
            enginePtr->getSurfaceCapabilities().currentExtent.width,
            enginePtr->getSurfaceCapabilities().currentExtent.height, 1)));
  }

  std::vector<GraphicsPipelineGroup::PipelineShaderStageCreateInfoParam>
      pipelineShaderStageCreateInfoList;

  for (auto &pair : this->shaderStageModulePtrMap) {
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
            this->renderPassPtr->getRenderPassHandleRef(), 0, VK_NULL_HANDLE,
            0}}));
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

std::shared_ptr<RenderPass> MaterialRaster::getRenderPassPtr() {
  return this->renderPassPtr;
}

std::vector<std::shared_ptr<Framebuffer>>
MaterialRaster::getFramebufferPtrList() {
  return this->framebufferPtrList;
}
