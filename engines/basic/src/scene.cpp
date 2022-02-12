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
    std::string materialName,
    std::map<Material::ShaderStage, std::string> shaderStageNameMap,
    bool isUsingRayTracingPipeline) {

  if (!isUsingRayTracingPipeline) {
    this->materialPtrList.push_back(std::shared_ptr<Material>(
        new MaterialRaster(this->enginePtr, materialName, shaderStageNameMap)));
  } else {
    this->materialPtrList.push_back(
        std::shared_ptr<Material>(new MaterialRayTrace(
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

  auto renderPassCommandBufferInheritanceInfoParamPtr =
      std::make_shared<CommandBufferGroup::CommandBufferInheritanceInfoParam>(
          CommandBufferGroup::CommandBufferInheritanceInfoParam{
              .renderPassHandle =
                  this->enginePtr->getRenderPassPtr()->getRenderPassHandleRef(),
              .subpass = 0,
              .framebufferHandle =
                  this->enginePtr->getFramebufferPtrList()[frameIndex]
                      ->getFramebufferHandleRef(),
              .occlusionQueryEnable = VK_FALSE,
              .queryControlFlags = 0,
              .queryPipelineStatisticFlags = 0});

  std::vector<VkCommandBuffer> commandBufferHandleList;
  std::vector<VkCommandBuffer> renderPassCommandBufferHandleList;
  for (auto &pair : this->indexModelMap) {
    if (pair.second->getMaterialPtr()->getMaterialType() ==
        Material::MaterialType::RASTER) {
      renderPassCommandBufferHandleList.push_back(
          enginePtr->getSecondaryCommandBufferGroupPtr()
              ->getCommandBufferHandleRef(
                  (this->enginePtr->getFramebufferPtrList().size() *
                   pair.first) +
                  frameIndex));
      pair.second->render(
          renderPassCommandBufferInheritanceInfoParamPtr,
          (this->enginePtr->getFramebufferPtrList().size() * pair.first) +
              frameIndex);
    } else {
      commandBufferHandleList.push_back(
          enginePtr->getSecondaryCommandBufferGroupPtr()
              ->getCommandBufferHandleRef(
                  (this->enginePtr->getFramebufferPtrList().size() *
                   pair.first) +
                  frameIndex));
      pair.second->render(
          commandBufferInheritanceInfoParamPtr,
          (this->enginePtr->getFramebufferPtrList().size() * pair.first) +
              frameIndex);
    }
  }

  this->enginePtr->getCommandBufferGroupPtr()->beginRecording(
      frameIndex, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

  if (commandBufferHandleList.size() > 0) {
    this->enginePtr->getCommandBufferGroupPtr()->executeCommandsCmd(
        frameIndex, commandBufferHandleList);
  }

  if (renderPassCommandBufferHandleList.size() > 0) {
    VkClearValue clearColor = {.color = {0.0, 0.0, 0.0, 1.0}};
    VkClearValue clearDepth = {.depthStencil = {.depth = 1.0}};

    this->enginePtr->getRenderPassPtr()->beginRenderPassCmd(
        this->enginePtr->getCommandBufferGroupPtr()->getCommandBufferHandleRef(
            frameIndex),
        this->enginePtr->getFramebufferPtrList()[frameIndex]
            ->getFramebufferHandleRef(),
        {{0, 0}, {800, 600}}, {clearColor, clearDepth},
        VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    this->enginePtr->getCommandBufferGroupPtr()->executeCommandsCmd(
        frameIndex, renderPassCommandBufferHandleList);

    this->enginePtr->getRenderPassPtr()->endRenderPassCmd(
        this->enginePtr->getCommandBufferGroupPtr()->getCommandBufferHandleRef(
            frameIndex));
  }

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
