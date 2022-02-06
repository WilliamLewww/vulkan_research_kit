#include "basic/material.h"
#include "basic/engine.h"
#include "basic/scene.h"

Material::Material(std::shared_ptr<Engine> enginePtr, std::string materialName)
    : enginePtr(enginePtr), materialName(materialName) {}

Material::~Material() {}

uint32_t Material::getMaterialPropertiesCount() {
  return this->materialPropertiesCount;
}

uint32_t Material::getTextureCount() { return this->textureCount; }

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

void Material::updateLightDescriptorSet(std::shared_ptr<Light> lightPtr) {
  this->descriptorSetGroupPtr->updateDescriptorSets(
      {{0, 2, lightPtr->getLightIndex(), 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        NULL, lightPtr->getLightDescriptorBufferInfoPtr(), NULL}},
      {});
}

void Material::appendMaterialPropertiesDescriptors(
    std::vector<Properties> propertiesList) {

  void *hostMaterialPropertiesBuffer;
  this->materialPropertiesBufferPtr->mapMemory(&hostMaterialPropertiesBuffer, 0,
                                               32 * sizeof(Properties));

  for (uint32_t x = 0; x < propertiesList.size(); x++) {
    memcpy(
        &((Properties *)
              hostMaterialPropertiesBuffer)[this->materialPropertiesCount + x],
        &propertiesList[x], sizeof(Properties));
  }

  this->materialPropertiesBufferPtr->unmapMemory();

  for (uint32_t x = 0; x < propertiesList.size(); x++) {
    std::shared_ptr<VkDescriptorBufferInfo>
        materialPropertiesDescriptorBufferInfoPtr =
            std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
                .buffer =
                    this->materialPropertiesBufferPtr->getBufferHandleRef(),
                .offset =
                    (this->materialPropertiesCount + x) * sizeof(Properties),
                .range = sizeof(Properties)});

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{0, 3, this->materialPropertiesCount + x, 1,
          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, NULL,
          materialPropertiesDescriptorBufferInfoPtr, NULL}},
        {});
  }

  this->materialPropertiesCount += propertiesList.size();
}

void Material::appendTextureDescriptors(
    std::vector<std::shared_ptr<ImageView>> imageViewPtrList) {

  for (uint32_t x = 0; x < imageViewPtrList.size(); x++) {
    std::shared_ptr<VkDescriptorImageInfo> descriptorImageInfoPtr =
        std::make_shared<VkDescriptorImageInfo>(VkDescriptorImageInfo{
            .sampler = VK_NULL_HANDLE,
            .imageView = imageViewPtrList[x]->getImageViewHandleRef(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{0, 5, this->textureCount + x, 1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
          descriptorImageInfoPtr, NULL, NULL}},
        {});
  }

  this->textureCount += imageViewPtrList.size();
}

void Material::updateModelDescriptorSet(std::shared_ptr<Model> modelPtr) {
  this->descriptorSetGroupPtr->updateDescriptorSets(
      {{0, 6, modelPtr->getModelIndex(), 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        NULL, modelPtr->getModelDescriptorBufferInfoPtr(), NULL}},
      {});
}
