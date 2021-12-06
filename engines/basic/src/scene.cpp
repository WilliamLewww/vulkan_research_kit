#include "basic/scene.h"
#include "basic/engine.h"

Scene::Scene(std::string sceneName, std::shared_ptr<Engine> enginePtr)
    : sceneName(sceneName), enginePtr(enginePtr) {

  this->sceneBufferPtr = std::unique_ptr<Buffer>(new Buffer(
      enginePtr->devicePtr->getDeviceHandleRef(),
      *enginePtr->physicalDeviceHandlePtr.get(), 0,
      sizeof(SceneShaderStructure), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {enginePtr->queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  this->sceneDescriptorBufferInfoPtr =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
          .buffer = this->sceneBufferPtr->getBufferHandleRef(),
          .offset = 0,
          .range = VK_WHOLE_SIZE});

  this->lightsBufferPtr = std::shared_ptr<Buffer>(new Buffer(
      enginePtr->devicePtr->getDeviceHandleRef(),
      *enginePtr->physicalDeviceHandlePtr.get(), 0,
      sizeof(Light::LightShaderStructure) * 16,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      {enginePtr->queueFamilyIndex}, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  this->lightsDescriptorBufferInfoPtr =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
          .buffer = this->lightsBufferPtr->getBufferHandleRef(),
          .offset = 0,
          .range = VK_WHOLE_SIZE});

  void *hostSceneBuffer;
  this->sceneBufferPtr->mapMemory(&hostSceneBuffer, 0,
                                  sizeof(SceneShaderStructure));
  memcpy(&((SceneShaderStructure *)hostSceneBuffer)->lightCount,
         &this->sceneShaderStructure.lightCount, 1 * sizeof(uint32_t));
  this->sceneBufferPtr->unmapMemory();
}

Scene::~Scene() {}

std::shared_ptr<Material> Scene::createMaterial(std::string materialName,
                                                std::string vertexFileName,
                                                std::string fragmentFileName) {

  this->materialPtrList.push_back(std::shared_ptr<Material>(new Material(
      this->enginePtr, materialName, vertexFileName, fragmentFileName)));

  return this->materialPtrList[this->materialPtrList.size() - 1];
}

std::shared_ptr<Model>
Scene::createModel(std::string modelName, std::string modelPath,
                   std::shared_ptr<Material> materialPtr) {

  this->modelPtrList.push_back(std::shared_ptr<Model>(new Model(
      this->enginePtr, shared_from_this(), modelName, modelPath, materialPtr)));

  uint32_t modelIndex = -1;
  for (uint32_t x = 0;
       modelIndex == -1 && x < this->enginePtr->secondaryCommandBufferCount / 3;
       x++) {
    if (this->indexModelMap.find(x) == this->indexModelMap.end()) {
      modelIndex = x;
    }
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
              .renderPassHandle =
                  this->enginePtr->renderPassPtr->getRenderPassHandleRef(),
              .subpass = 0,
              .framebufferHandle =
                  this->enginePtr->framebufferPtrList[frameIndex]
                      ->getFramebufferHandleRef(),
              .occlusionQueryEnable = VK_FALSE,
              .queryControlFlags = 0,
              .queryPipelineStatisticFlags = 0});

  std::vector<VkCommandBuffer> commandBufferHandleList;
  for (auto &pair : this->indexModelMap) {
    commandBufferHandleList.push_back(
        enginePtr->secondaryCommandBufferGroupPtr->getCommandBufferHandleRef(
            (this->enginePtr->framebufferPtrList.size() * pair.first) +
            frameIndex));
    pair.second->render(
        commandBufferInheritanceInfoParamPtr,
        (this->enginePtr->framebufferPtrList.size() * pair.first) + frameIndex);
  }

  this->enginePtr->commandBufferGroupPtr->beginRecording(
      frameIndex, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

  this->enginePtr->renderPassPtr->beginRenderPassCmd(
      this->enginePtr->commandBufferGroupPtr->getCommandBufferHandleRef(
          frameIndex),
      this->enginePtr->framebufferPtrList[frameIndex]
          ->getFramebufferHandleRef(),
      {{0, 0}, {800, 600}}, {{{0.0, 0.0, 0.0, 1.0}}},
      VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

  this->enginePtr->commandBufferGroupPtr->executeCommandsCmd(
      frameIndex, commandBufferHandleList);

  this->enginePtr->renderPassPtr->endRenderPassCmd(
      this->enginePtr->commandBufferGroupPtr->getCommandBufferHandleRef(
          frameIndex));

  this->enginePtr->commandBufferGroupPtr->endRecording(frameIndex);
}

std::vector<std::shared_ptr<Material>> Scene::getMaterialPtrList() {
  return this->materialPtrList;
}

std::shared_ptr<VkDescriptorBufferInfo>
Scene::getSceneDescriptorBufferInfoPtr() {
  return this->sceneDescriptorBufferInfoPtr;
}

std::shared_ptr<VkDescriptorBufferInfo>
Scene::getLightsDescriptorBufferInfoPtr() {
  return this->lightsDescriptorBufferInfoPtr;
}
