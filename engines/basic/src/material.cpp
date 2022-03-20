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

    this->shaderStageModulePtrMap[pair.first] =
        std::unique_ptr<ShaderModule>(new ShaderModule(
            enginePtr->getDevicePtr()->getDeviceHandleRef(), shaderSource));
  }
}

void Material::initializeDescriptors(
    const MaterialDescriptorCounts materialDescriptorCounts,
    std::shared_ptr<DescriptorSetLayout> descriptorSetLayoutPtr) {

  std::vector<VkDescriptorPoolSize> descriptorPoolSizeList = {};

  uint32_t uniformBufferCount =
      this->SHARED_DESCRIPTOR_COUNTS.uniformBufferCount +
      this->MATERIAL_DESCRIPTOR_COUNTS.uniformBufferCount +
      materialDescriptorCounts.uniformBufferCount;
  if (uniformBufferCount > 0) {
    descriptorPoolSizeList.push_back(
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBufferCount});
  }

  uint32_t storageBufferCount =
      this->SHARED_DESCRIPTOR_COUNTS.storageBufferCount +
      this->MATERIAL_DESCRIPTOR_COUNTS.storageBufferCount +
      materialDescriptorCounts.storageBufferCount;
  if (storageBufferCount > 0) {
    descriptorPoolSizeList.push_back(
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, storageBufferCount});
  }

  uint32_t samplerCount = this->SHARED_DESCRIPTOR_COUNTS.samplerCount +
                          this->MATERIAL_DESCRIPTOR_COUNTS.samplerCount +
                          materialDescriptorCounts.samplerCount;
  if (samplerCount > 0) {
    descriptorPoolSizeList.push_back(
        {VK_DESCRIPTOR_TYPE_SAMPLER, samplerCount});
  }

  uint32_t sampledImageCount =
      this->SHARED_DESCRIPTOR_COUNTS.sampledImageCount +
      this->MATERIAL_DESCRIPTOR_COUNTS.sampledImageCount +
      materialDescriptorCounts.sampledImageCount;
  if (sampledImageCount > 0) {
    descriptorPoolSizeList.push_back(
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, sampledImageCount});
  }

  uint32_t storageImageCount =
      this->SHARED_DESCRIPTOR_COUNTS.storageImageCount +
      this->MATERIAL_DESCRIPTOR_COUNTS.storageImageCount +
      materialDescriptorCounts.storageImageCount;
  if (storageImageCount > 0) {
    descriptorPoolSizeList.push_back(
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, storageImageCount});
  }

  uint32_t accelerationStructureCount =
      this->SHARED_DESCRIPTOR_COUNTS.accelerationStructureCount +
      this->MATERIAL_DESCRIPTOR_COUNTS.accelerationStructureCount +
      materialDescriptorCounts.accelerationStructureCount;
  if (accelerationStructureCount > 0) {
    descriptorPoolSizeList.push_back(
        {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
         accelerationStructureCount});
  }

  this->descriptorPoolPtr = std::unique_ptr<DescriptorPool>(
      new DescriptorPool(enginePtr->getDevicePtr()->getDeviceHandleRef(),
                         VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 3,
                         descriptorPoolSizeList));

  this->sharedDescriptorSetLayoutPtr =
      std::unique_ptr<DescriptorSetLayout>(new DescriptorSetLayout(
          enginePtr->getDevicePtr()->getDeviceHandleRef(), 0,
          {{0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 8,
            VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
            NULL}}));

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
      std::shared_ptr<DescriptorSetGroup>(new DescriptorSetGroup(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          this->descriptorPoolPtr->getDescriptorPoolHandleRef(),
          {this->sharedDescriptorSetLayoutPtr
               ->getDescriptorSetLayoutHandleRef(),
           this->descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef(),
           descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef()}));

  this->pipelineLayoutPtr = std::unique_ptr<PipelineLayout>(new PipelineLayout(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      {this->sharedDescriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef(),
       this->descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef(),
       descriptorSetLayoutPtr->getDescriptorSetLayoutHandleRef()},
      {}));

  this->materialPropertiesCount = 0;

  this->materialPropertiesBufferPtr = std::shared_ptr<Buffer>(new Buffer(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
      sizeof(Model::Properties) * 32, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
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

  for (uint32_t x = 0; x < 8; x++) {
    VkDescriptorImageInfo descriptorImageInfo = {.sampler = VK_NULL_HANDLE,
                                                 .imageView = VK_NULL_HANDLE,
                                                 .imageLayout =
                                                     VK_IMAGE_LAYOUT_UNDEFINED};

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{0,
          0,
          x,
          1,
          VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
          {descriptorImageInfo},
          {},
          {}}},
        {});
  }

  for (uint32_t x = 0; x < 16; x++) {
    VkDescriptorBufferInfo lightDescriptorBufferInfo = {
        .buffer = VK_NULL_HANDLE, .offset = 0, .range = VK_WHOLE_SIZE};

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{1,
          2,
          x,
          1,
          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          {},
          {lightDescriptorBufferInfo},
          {}}},
        {});
  }

  for (uint32_t x = 0; x < 32; x++) {
    VkDescriptorBufferInfo materialPropertiesDescriptorBufferInfo = {
        .buffer = VK_NULL_HANDLE, .offset = 0, .range = VK_WHOLE_SIZE};

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{1,
          3,
          x,
          1,
          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
          {},
          {materialPropertiesDescriptorBufferInfo},
          {}}},
        {});
  }

  VkDescriptorImageInfo descriptorSamplerImageInfo = {
      .sampler = samplerPtr->getSamplerHandleRef(),
      .imageView = VK_NULL_HANDLE,
      .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED};

  descriptorSetGroupPtr->updateDescriptorSets({{1,
                                                4,
                                                0,
                                                1,
                                                VK_DESCRIPTOR_TYPE_SAMPLER,
                                                {descriptorSamplerImageInfo},
                                                {},
                                                {}}},
                                              {});

  for (uint32_t x = 0; x < 32; x++) {
    VkDescriptorImageInfo descriptorImageInfo = {.sampler = VK_NULL_HANDLE,
                                                 .imageView = VK_NULL_HANDLE,
                                                 .imageLayout =
                                                     VK_IMAGE_LAYOUT_UNDEFINED};

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{1,
          5,
          x,
          1,
          VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
          {descriptorImageInfo},
          {},
          {}}},
        {});
  }

  for (uint32_t x = 0; x < 32; x++) {
    VkDescriptorBufferInfo modelDescriptorBufferInfo = {
        .buffer = VK_NULL_HANDLE, .offset = 0, .range = VK_WHOLE_SIZE};

    this->descriptorSetGroupPtr->updateDescriptorSets(
        {{1,
          6,
          x,
          1,
          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          {},
          {modelDescriptorBufferInfo},
          {}}},
        {});
  }
}

Material::~Material() {}

Material::MaterialType Material::getMaterialType() {
  return this->materialType;
}

void Material::incrementMaterialPropertiesCount(uint32_t count) {
  this->materialPropertiesCount += count;
}

uint32_t Material::getMaterialPropertiesCount() {
  return this->materialPropertiesCount;
}

std::shared_ptr<Buffer> Material::getMaterialPropertiesBufferPtr() {
  return this->materialPropertiesBufferPtr;
}

void Material::incrementTextureCount(uint32_t count) {
  this->textureCount += count;
}

uint32_t Material::getTextureCount() { return this->textureCount; }

std::shared_ptr<Image> Material::getImagePtr(std::string imageName) {
  return this->imagePtrMap[imageName];
}

std::shared_ptr<ImageView> Material::getImageViewPtr(std::string imageName) {
  return this->imageViewPtrMap[imageName];
}

std::shared_ptr<DescriptorSetGroup> Material::getDescriptorSetGroupPtr() {
  return this->descriptorSetGroupPtr;
}

void Material::updateCameraDescriptorSet(std::shared_ptr<Camera> cameraPtr) {
  this->descriptorSetGroupPtr->updateDescriptorSets(
      {{1,
        0,
        0,
        1,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        {},
        {*cameraPtr->getCameraDescriptorBufferInfoPtr().get()},
        {}}},
      {});
}

void Material::updateSceneDescriptorSet(std::shared_ptr<Scene> scenePtr) {
  this->descriptorSetGroupPtr->updateDescriptorSets(
      {{1,
        1,
        0,
        1,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        {},
        {*scenePtr->getSceneDescriptorBufferInfoPtr().get()},
        {}}},
      {});
}

void Material::updateLightDescriptorSet(std::shared_ptr<Light> lightPtr) {
  this->descriptorSetGroupPtr->updateDescriptorSets(
      {{1,
        2,
        lightPtr->getLightIndex(),
        1,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        {},
        {*lightPtr->getLightDescriptorBufferInfoPtr().get()},
        {}}},
      {});
}

void Material::updateModelDescriptorSet(std::shared_ptr<Model> modelPtr) {
  this->descriptorSetGroupPtr->updateDescriptorSets(
      {{1,
        6,
        modelPtr->getModelIndex(),
        1,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        {},
        {*modelPtr->getModelDescriptorBufferInfoPtr().get()},
        {}}},
      {});
}
