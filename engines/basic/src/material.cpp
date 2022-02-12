#include "basic/material.h"
#include "basic/engine.h"
#include "basic/scene.h"

Material::Material(std::shared_ptr<Engine> enginePtr, std::string materialName,
                   std::map<ShaderStage, std::string> shaderStageNameMap,
                   MaterialType materialType)
    : enginePtr(enginePtr), materialName(materialName),
      materialType(materialType) {

  for (auto &pair : shaderStageNameMap) {
    std::ifstream shaderFile(Resource::findResource(pair.second),
                             std::ios::binary | std::ios::ate);
    std::streamsize shaderFileSize = shaderFile.tellg();
    shaderFile.seekg(0, std::ios::beg);
    std::vector<uint32_t> shaderSource(shaderFileSize / sizeof(uint32_t));
    shaderFile.read((char *)shaderSource.data(), shaderFileSize);
    shaderFile.close();

    this->shaderStageModuleMap[pair.first] =
        std::unique_ptr<ShaderModule>(new ShaderModule(
            enginePtr->getDevicePtr()->getDeviceHandleRef(), shaderSource));
  }
}

void Material::initializeDescriptors(
    const MaterialDescriptorCounts materialDescriptorCounts,
    std::shared_ptr<DescriptorSetLayout> descriptorSetLayoutPtr) {

  this->descriptorPoolPtr = std::unique_ptr<DescriptorPool>(
      new DescriptorPool(enginePtr->getDevicePtr()->getDeviceHandleRef(),
                         VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 2,
                         {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                           this->MATERIAL_DESCRIPTOR_COUNTS.uniformBufferCount +
                               materialDescriptorCounts.uniformBufferCount},
                          {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                           this->MATERIAL_DESCRIPTOR_COUNTS.storageBufferCount +
                               materialDescriptorCounts.storageBufferCount},
                          {VK_DESCRIPTOR_TYPE_SAMPLER,
                           this->MATERIAL_DESCRIPTOR_COUNTS.samplerCount +
                               materialDescriptorCounts.samplerCount},
                          {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                           this->MATERIAL_DESCRIPTOR_COUNTS.sampledImageCount +
                               materialDescriptorCounts.sampledImageCount}}));

  this->descriptorSetLayoutPtr =
      std::unique_ptr<DescriptorSetLayout>(new DescriptorSetLayout(
          enginePtr->getDevicePtr()->getDeviceHandleRef(), 0,
          {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT |
                VK_SHADER_STAGE_FRAGMENT_BIT,
            NULL},
           {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
            VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
           {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16,
            VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
           {3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 32,
            VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
           {4, VK_DESCRIPTOR_TYPE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
            NULL},
           {5, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 32,
            VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
           {6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 32,
            VK_SHADER_STAGE_VERTEX_BIT, NULL}}));

  this->descriptorSetGroupPtr =
      std::unique_ptr<DescriptorSetGroup>(new DescriptorSetGroup(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          this->descriptorPoolPtr->getDescriptorPoolHandleRef(),
          {this->descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef(),
           descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef()}));

  this->pipelineLayoutPtr = std::unique_ptr<PipelineLayout>(new PipelineLayout(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      {this->descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef(),
       descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef()},
      {}));

  this->materialPropertiesCount = 0;

  this->materialPropertiesBufferPtr = std::unique_ptr<Buffer>(
      new Buffer(enginePtr->getDevicePtr()->getDeviceHandleRef(),
                 *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
                 sizeof(Properties) * 32, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                 VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  this->samplerPtr = std::unique_ptr<Sampler>(new Sampler(
      enginePtr->getDevicePtr()->getDeviceHandleRef(), 0, VK_FILTER_NEAREST,
      VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST,
      VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT,
      VK_SAMPLER_ADDRESS_MODE_REPEAT, 0, VK_FALSE, 0, VK_FALSE,
      VK_COMPARE_OP_NEVER, 0, 0, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
      VK_FALSE));

  this->textureCount = 0;

  for (uint32_t x = 0; x < 16; x++) {
    std::shared_ptr<VkDescriptorBufferInfo> lightDescriptorBufferInfoPtr =
        std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
            .buffer = VK_NULL_HANDLE, .offset = 0, .range = VK_WHOLE_SIZE});

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{0, 2, x, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL,
          lightDescriptorBufferInfoPtr, NULL}},
        {});
  }

  for (uint32_t x = 0; x < 32; x++) {
    std::shared_ptr<VkDescriptorBufferInfo>
        materialPropertiesDescriptorBufferInfoPtr =
            std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
                .buffer = VK_NULL_HANDLE, .offset = 0, .range = VK_WHOLE_SIZE});

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{0, 3, x, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, NULL,
          materialPropertiesDescriptorBufferInfoPtr, NULL}},
        {});
  }

  std::shared_ptr<VkDescriptorImageInfo> descriptorSamplerImageInfoPtr =
      std::make_shared<VkDescriptorImageInfo>(
          VkDescriptorImageInfo{.sampler = samplerPtr->getSamplerHandleRef(),
                                .imageView = VK_NULL_HANDLE,
                                .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED});

  descriptorSetGroupPtr->updateDescriptorSets(
      {{0, 4, 0, 1, VK_DESCRIPTOR_TYPE_SAMPLER, descriptorSamplerImageInfoPtr,
        NULL, NULL}},
      {});

  for (uint32_t x = 0; x < 32; x++) {
    std::shared_ptr<VkDescriptorImageInfo> descriptorImageInfoPtr =
        std::make_shared<VkDescriptorImageInfo>(
            VkDescriptorImageInfo{.sampler = VK_NULL_HANDLE,
                                  .imageView = VK_NULL_HANDLE,
                                  .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED});

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{0, 5, x, 1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptorImageInfoPtr,
          NULL, NULL}},
        {});
  }

  for (uint32_t x = 0; x < 32; x++) {
    std::shared_ptr<VkDescriptorBufferInfo> modelDescriptorBufferInfoPtr =
        std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
            .buffer = VK_NULL_HANDLE, .offset = 0, .range = VK_WHOLE_SIZE});

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{0, 6, x, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL,
          modelDescriptorBufferInfoPtr, NULL}},
        {});
  }
}

Material::~Material() {}

Material::MaterialType Material::getMaterialType() {
  return this->materialType;
}

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
