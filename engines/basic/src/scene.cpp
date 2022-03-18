#include "basic/scene.h"
#include "basic/engine.h"

Scene::Scene(std::string sceneName, std::shared_ptr<Engine> enginePtr)
    : sceneName(sceneName), enginePtr(enginePtr) {

  this->sceneShaderStructure = {};

  this->sceneBufferPtr = std::unique_ptr<Buffer>(new Buffer(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
      sizeof(SceneShaderStructure), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  this->sceneDescriptorBufferInfoPtr =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
          .buffer = this->sceneBufferPtr->getBufferHandleRef(),
          .offset = 0,
          .range = VK_WHOLE_SIZE});

  this->lightsBufferPtr = std::shared_ptr<Buffer>(new Buffer(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
      sizeof(Light::LightShaderStructure) * 16,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      {enginePtr->getQueueFamilyIndex()}, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  this->modelsBufferPtr = std::shared_ptr<Buffer>(new Buffer(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
      sizeof(Model::ModelShaderStructure) * 32,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      {enginePtr->getQueueFamilyIndex()}, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  void *hostSceneBuffer;
  this->sceneBufferPtr->mapMemory(&hostSceneBuffer, 0,
                                  sizeof(SceneShaderStructure));
  memcpy(&((SceneShaderStructure *)hostSceneBuffer)->lightCount,
         &this->sceneShaderStructure.lightCount, 1 * sizeof(uint32_t));
  this->sceneBufferPtr->unmapMemory();
}

Scene::~Scene() {}

std::shared_ptr<Material> Scene::createMaterial(
    std::string materialName, Material::MaterialType materialType,
    std::map<Material::ShaderStage, std::string> shaderStageNameMap) {

  if (materialType == Material::MaterialType::RASTER) {
    this->materialPtrList.push_back(std::shared_ptr<Material>(
        new MaterialRaster(this->enginePtr, materialName, shaderStageNameMap)));
  } else if (materialType == Material::MaterialType::RAY_TRACE) {
    this->materialPtrList.push_back(
        std::shared_ptr<Material>(new MaterialRayTrace(
            this->enginePtr, materialName, shaderStageNameMap)));
  } else if (materialType == Material::MaterialType::COMPUTE) {
    this->materialPtrList.push_back(
        std::shared_ptr<Material>(new MaterialCompute(
            this->enginePtr, materialName, shaderStageNameMap)));
  }

  return this->materialPtrList[this->materialPtrList.size() - 1];
}

std::shared_ptr<Model>
Scene::createModel(std::string modelName, std::string modelPath,
                   std::shared_ptr<Material> materialPtr) {

  uint32_t modelIndex = -1;
  for (uint32_t x = 0;
       modelIndex == -1 &&
       x < this->enginePtr->getSecondaryCommandBufferCount() / 3;
       x++) {
    if (this->indexModelMap.find(x) == this->indexModelMap.end()) {
      modelIndex = x;
    }
  }

  this->modelPtrList.push_back(std::shared_ptr<Model>(
      new Model(this->enginePtr, shared_from_this(), modelName, modelPath,
                materialPtr, modelIndex, this->modelsBufferPtr)));

  if (materialPtr->getMaterialType() == Material::MaterialType::RAY_TRACE) {
    std::static_pointer_cast<MaterialRayTrace>(materialPtr)
        ->createBottomLevelAccelerationStructure(
            this->modelPtrList[this->modelPtrList.size() - 1]);
  }

  this->indexModelMap[modelIndex] =
      this->modelPtrList[this->modelPtrList.size() - 1];

  return this->modelPtrList[this->modelPtrList.size() - 1];
}

std::shared_ptr<Light> Scene::createLight(std::string lightName,
                                          Light::LightType lightType) {
  this->sceneShaderStructure.lightCount += 1;

  void *hostSceneBuffer;
  this->sceneBufferPtr->mapMemory(&hostSceneBuffer, 0,
                                  sizeof(SceneShaderStructure));
  memcpy((uint32_t *)hostSceneBuffer + 0,
         &this->sceneShaderStructure.lightCount, 1 * sizeof(uint32_t));
  this->sceneBufferPtr->unmapMemory();

  this->lightPtrList.push_back(std::shared_ptr<Light>(
      new Light(this->enginePtr, this->lightsBufferPtr,
                this->lightPtrList.size(), lightName, lightType)));

  return this->lightPtrList[this->lightPtrList.size() - 1];
}

void Scene::appendToRenderQueue(std::shared_ptr<Model> modelPtr) {
  RenderQueueEntry renderQueueEntry = {.renderQueueEntryType =
                                           RenderQueueEntryType::MODEL,
                                       .entryPtr = modelPtr};

  this->renderQueueEntryList.push_back(renderQueueEntry);
}

void Scene::appendToRenderQueue(std::shared_ptr<Material> materialPtr) {
  RenderQueueEntry renderQueueEntry = {.renderQueueEntryType =
                                           RenderQueueEntryType::MATERIAL,
                                       .entryPtr = materialPtr};

  this->renderQueueEntryList.push_back(renderQueueEntry);
}

void Scene::recordCommandBuffer(uint32_t frameIndex) {
  auto commandBufferInheritanceInfoParamPtr =
      std::make_shared<CommandBufferGroup::CommandBufferInheritanceInfoParam>(
          CommandBufferGroup::CommandBufferInheritanceInfoParam{
              .renderPassHandle = VK_NULL_HANDLE,
              .subpass = 0,
              .framebufferHandle = VK_NULL_HANDLE,
              .occlusionQueryEnable = VK_FALSE,
              .queryControlFlags = 0,
              .queryPipelineStatisticFlags = 0});

  std::vector<VkCommandBuffer> commandBufferHandleList;
  for (uint32_t x = 0; x < this->renderQueueEntryList.size(); x++) {
    if (this->renderQueueEntryList[x].renderQueueEntryType ==
        RenderQueueEntryType::MODEL) {

      std::shared_ptr<Model> modelPtr = std::static_pointer_cast<Model>(
          this->renderQueueEntryList[x].entryPtr);

      if (modelPtr->getMaterialPtr()->getMaterialType() ==
          Material::MaterialType::RASTER) {

        std::shared_ptr<MaterialRaster> materialRasterPtr =
            std::static_pointer_cast<MaterialRaster>(
                modelPtr->getMaterialPtr());

        auto renderPassCommandBufferInheritanceInfoParamPtr = std::make_shared<
            CommandBufferGroup::CommandBufferInheritanceInfoParam>(
            CommandBufferGroup::CommandBufferInheritanceInfoParam{
                .renderPassHandle = materialRasterPtr->getRenderPassPtr()
                                        ->getRenderPassHandleRef(),
                .subpass = 0,
                .framebufferHandle =
                    materialRasterPtr->getFramebufferPtrList()[frameIndex]
                        ->getFramebufferHandleRef(),
                .occlusionQueryEnable = VK_FALSE,
                .queryControlFlags = 0,
                .queryPipelineStatisticFlags = 0});

        commandBufferHandleList.push_back(
            enginePtr->getSecondaryCommandBufferGroupPtr()
                ->getCommandBufferHandleRef(
                    (this->enginePtr->getSwapchainImageCount() * x) +
                    frameIndex));

        modelPtr->render(renderPassCommandBufferInheritanceInfoParamPtr,
                         (this->enginePtr->getSwapchainImageCount() * x) +
                             frameIndex);
      } else if (modelPtr->getMaterialPtr()->getMaterialType() ==
                 Material::MaterialType::RAY_TRACE) {

        VkDescriptorImageInfo descriptorImageInfo = {
            .sampler = VK_NULL_HANDLE,
            .imageView = modelPtr->getMaterialPtr()
                             ->getImageViewPtr("output" + frameIndex)
                             ->getImageViewHandleRef(),
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL};

        modelPtr->getMaterialPtr()
            ->getDescriptorSetGroupPtr()
            ->updateDescriptorSets({{1,
                                     0,
                                     0,
                                     1,
                                     VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                     {descriptorImageInfo},
                                     {},
                                     {}}},
                                   {});

        commandBufferHandleList.push_back(
            enginePtr->getSecondaryCommandBufferGroupPtr()
                ->getCommandBufferHandleRef(
                    (this->enginePtr->getSwapchainImageCount() * x) +
                    frameIndex));

        modelPtr->render(commandBufferInheritanceInfoParamPtr,
                         (this->enginePtr->getSwapchainImageCount() * x) +
                             frameIndex);
      }
    } else if (this->renderQueueEntryList[x].renderQueueEntryType ==
               RenderQueueEntryType::MATERIAL) {

      std::shared_ptr<Material> materialPtr =
          std::static_pointer_cast<Material>(
              this->renderQueueEntryList[x].entryPtr);

      VkDescriptorImageInfo descriptorImageInfo = {
          .sampler = VK_NULL_HANDLE,
          .imageView = materialPtr->getImageViewPtr("output" + frameIndex)
                           ->getImageViewHandleRef(),
          .imageLayout = VK_IMAGE_LAYOUT_GENERAL};

      materialPtr->getDescriptorSetGroupPtr()->updateDescriptorSets(
          {{1,
            0,
            0,
            1,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            {descriptorImageInfo},
            {},
            {}}},
          {});

      commandBufferHandleList.push_back(
          enginePtr->getSecondaryCommandBufferGroupPtr()
              ->getCommandBufferHandleRef(
                  (this->enginePtr->getSwapchainImageCount() * x) +
                  frameIndex));

      this->enginePtr->getSecondaryCommandBufferGroupPtr()->beginRecording(
          (this->enginePtr->getSwapchainImageCount() * x) + frameIndex,
          (VkCommandBufferUsageFlagBits)0,
          commandBufferInheritanceInfoParamPtr);

      materialPtr->render(
          this->enginePtr->getSecondaryCommandBufferGroupPtr()
              ->getCommandBufferHandleRef(
                  (this->enginePtr->getSwapchainImageCount() * x) + frameIndex),
          NULL);

      this->enginePtr->getSecondaryCommandBufferGroupPtr()->endRecording(
          (this->enginePtr->getSwapchainImageCount() * x) + frameIndex);
    }
  }

  this->enginePtr->getCommandBufferGroupPtr()->beginRecording(
      frameIndex, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

  for (uint32_t x = 0; x < this->renderQueueEntryList.size(); x++) {
    if (this->renderQueueEntryList[x].renderQueueEntryType ==
        RenderQueueEntryType::MODEL) {
      if (std::static_pointer_cast<Model>(
              this->renderQueueEntryList[x].entryPtr)
              ->getMaterialPtr()
              ->getMaterialType() == Material::MaterialType::RASTER) {

        std::shared_ptr<MaterialRaster> materialRasterPtr =
            std::static_pointer_cast<MaterialRaster>(
                std::static_pointer_cast<Model>(
                    this->renderQueueEntryList[x].entryPtr)
                    ->getMaterialPtr());

        VkClearValue clearColor = {.color = {0.0, 0.0, 0.0, 1.0}};
        VkClearValue clearDepth = {.depthStencil = {.depth = 1.0}};

        materialRasterPtr->getRenderPassPtr()->beginRenderPassCmd(
            this->enginePtr->getCommandBufferGroupPtr()
                ->getCommandBufferHandleRef(frameIndex),
            materialRasterPtr->getFramebufferPtrList()[frameIndex]
                ->getFramebufferHandleRef(),
            {{0, 0}, {800, 600}}, {clearColor, clearDepth},
            VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

        std::vector<VkCommandBuffer> combinedCommandBufferHandleList = {
            commandBufferHandleList[x]};

        for (uint32_t y = x + 1; y < this->renderQueueEntryList.size(); y++) {
          if (this->renderQueueEntryList[y].renderQueueEntryType ==
                  RenderQueueEntryType::MODEL &&
              std::static_pointer_cast<Model>(
                  this->renderQueueEntryList[y].entryPtr)
                      ->getMaterialPtr() == materialRasterPtr) {

            combinedCommandBufferHandleList.push_back(
                commandBufferHandleList[y]);
            x += 1;
          } else {
            break;
          }
        }

        this->enginePtr->getCommandBufferGroupPtr()->executeCommandsCmd(
            frameIndex, combinedCommandBufferHandleList);

        materialRasterPtr->getRenderPassPtr()->endRenderPassCmd(
            this->enginePtr->getCommandBufferGroupPtr()
                ->getCommandBufferHandleRef(frameIndex));
      } else if (std::static_pointer_cast<Model>(
                     this->renderQueueEntryList[x].entryPtr)
                     ->getMaterialPtr()
                     ->getMaterialType() == Material::MaterialType::RAY_TRACE) {

        std::shared_ptr<Image> sourceImagePtr =
            std::static_pointer_cast<Model>(
                this->renderQueueEntryList[x].entryPtr)
                ->getMaterialPtr()
                ->getImagePtr("output" + frameIndex);

        this->enginePtr->getCommandBufferGroupPtr()->createPipelineBarrierCmd(
            frameIndex, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, {}, {},
            {{VK_ACCESS_MEMORY_WRITE_BIT,
              0,
              VK_IMAGE_LAYOUT_UNDEFINED,
              VK_IMAGE_LAYOUT_GENERAL,
              this->enginePtr->getQueueFamilyIndex(),
              this->enginePtr->getQueueFamilyIndex(),
              sourceImagePtr->getImageHandleRef(),
              {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

        this->enginePtr->getCommandBufferGroupPtr()->executeCommandsCmd(
            frameIndex, {commandBufferHandleList[x]});
      }
    } else {
      std::shared_ptr<Image> sourceImagePtr =
          std::static_pointer_cast<Material>(
              this->renderQueueEntryList[x].entryPtr)
              ->getImagePtr("output" + frameIndex);

      this->enginePtr->getCommandBufferGroupPtr()->createPipelineBarrierCmd(
          frameIndex, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
          VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, {}, {},
          {{VK_ACCESS_MEMORY_WRITE_BIT,
            VK_ACCESS_MEMORY_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_GENERAL,
            this->enginePtr->getQueueFamilyIndex(),
            this->enginePtr->getQueueFamilyIndex(),
            sourceImagePtr->getImageHandleRef(),
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

      this->enginePtr->getCommandBufferGroupPtr()->executeCommandsCmd(
          frameIndex, {commandBufferHandleList[x]});
    }
  }

  std::shared_ptr<Image> sourceImagePtr;
  VkPipelineStageFlagBits pipelineStageFlagBits =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkAccessFlagBits accessFlagBits = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  if (this->renderQueueEntryList.back().renderQueueEntryType ==
      RenderQueueEntryType::MODEL) {

    sourceImagePtr = std::static_pointer_cast<Model>(
                         this->renderQueueEntryList.back().entryPtr)
                         ->getMaterialPtr()
                         ->getImagePtr("output" + frameIndex);

  } else if (this->renderQueueEntryList.back().renderQueueEntryType ==
             RenderQueueEntryType::MATERIAL) {

    sourceImagePtr = std::static_pointer_cast<Material>(
                         this->renderQueueEntryList.back().entryPtr)
                         ->getImagePtr("output" + frameIndex);

    pipelineStageFlagBits = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    accessFlagBits = VK_ACCESS_SHADER_WRITE_BIT;
  }

  this->enginePtr->getCommandBufferGroupPtr()->createPipelineBarrierCmd(
      frameIndex, pipelineStageFlagBits, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, {},
      {},
      {{accessFlagBits,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        this->enginePtr->getQueueFamilyIndex(),
        this->enginePtr->getQueueFamilyIndex(),
        sourceImagePtr->getImageHandleRef(),
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}},
       {accessFlagBits,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        this->enginePtr->getQueueFamilyIndex(),
        this->enginePtr->getQueueFamilyIndex(),
        this->enginePtr->getSwapchainImagePtrList()[frameIndex]
            ->getImageHandleRef(),
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

  this->enginePtr->getSwapchainImagePtrList()[frameIndex]->copyFromImageCmd(
      this->enginePtr->getCommandBufferGroupPtr()->getCommandBufferHandleRef(
          frameIndex),
      sourceImagePtr->getImageHandleRef(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      {{{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        {0, 0, 0},
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        {0, 0, 0},
        {800, 600, 1}}});

  this->enginePtr->getCommandBufferGroupPtr()->createPipelineBarrierCmd(
      frameIndex, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, {}, {},
      {{VK_ACCESS_TRANSFER_WRITE_BIT,
        0,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        this->enginePtr->getQueueFamilyIndex(),
        this->enginePtr->getQueueFamilyIndex(),
        this->enginePtr->getSwapchainImagePtrList()[frameIndex]
            ->getImageHandleRef(),
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

  this->enginePtr->getCommandBufferGroupPtr()->endRecording(frameIndex);
}

std::vector<std::shared_ptr<Material>> Scene::getMaterialPtrList() {
  return this->materialPtrList;
}

std::vector<std::shared_ptr<Light>> Scene::getLightPtrList() {
  return this->lightPtrList;
}

std::vector<std::shared_ptr<Model>> Scene::getModelPtrList() {
  return this->modelPtrList;
}

std::shared_ptr<VkDescriptorBufferInfo>
Scene::getSceneDescriptorBufferInfoPtr() {
  return this->sceneDescriptorBufferInfoPtr;
}
