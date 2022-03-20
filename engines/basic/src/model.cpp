#include "basic/model.h"
#include "basic/engine.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Model::Model(std::shared_ptr<Engine> enginePtr, std::shared_ptr<Scene> scenePtr,
             std::string modelName, std::string modelPath,
             std::shared_ptr<Material> materialPtr, uint32_t modelIndex,
             std::shared_ptr<Buffer> modelsBufferPtr)
    : enginePtr(enginePtr), scenePtr(scenePtr), modelName(modelName),
      materialPtr(materialPtr), modelIndex(modelIndex),
      modelsBufferPtr(modelsBufferPtr),
      materialOffsetIndex(materialPtr->getMaterialPropertiesCount()),
      materialPropertiesBufferPtr(
          materialPtr->getMaterialPropertiesBufferPtr()),
      textureOffsetIndex(materialPtr->getTextureCount()) {

  this->modelShaderStructure = {};

  this->position[0] = 0.0;
  this->position[1] = 0.0;
  this->position[2] = 0.0;

  this->yaw = 0.0;
  this->pitch = 0.0;
  this->roll = 0.0;

  this->updateModelMatrix();

  this->modelDescriptorBufferInfoPtr =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{
          .buffer = modelsBufferPtr->getBufferHandleRef(),
          .offset = modelIndex * sizeof(ModelShaderStructure),
          .range = sizeof(ModelShaderStructure)});

  void *hostModelsBuffer;
  this->modelsBufferPtr->mapMemory(&hostModelsBuffer, 0,
                                   32 * sizeof(ModelShaderStructure));
  memcpy(&((ModelShaderStructure *)hostModelsBuffer)[modelIndex],
         &this->modelShaderStructure, sizeof(ModelShaderStructure));
  this->modelsBufferPtr->unmapMemory();

  std::string modelDirectory = modelPath.substr(0, modelPath.find_last_of("/"));

  tinyobj::ObjReaderConfig reader_config;
  reader_config.mtl_search_path = modelDirectory;

  tinyobj::ObjReader reader;
  if (!reader.ParseFromFile(modelPath, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    exit(1);
  }

  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader: " << reader.Warning();
  }

  auto &attrib = reader.GetAttrib();
  auto &shapes = reader.GetShapes();
  auto &materials = reader.GetMaterials();

  for (uint32_t s = 0; s < shapes.size(); s++) {
    uint32_t index_offset = 0;
    for (uint32_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      uint32_t fv = uint32_t(shapes[s].mesh.num_face_vertices[f]);
      for (uint32_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

        float vx = attrib.vertices[3 * uint32_t(idx.vertex_index) + 0];
        float vy = attrib.vertices[3 * uint32_t(idx.vertex_index) + 1];
        float vz = attrib.vertices[3 * uint32_t(idx.vertex_index) + 2];

        float nx = 0.0, ny = 0.0, nz = 0.0;
        if (idx.normal_index >= 0) {
          nx = attrib.normals[3 * uint32_t(idx.normal_index) + 0];
          ny = attrib.normals[3 * uint32_t(idx.normal_index) + 1];
          nz = attrib.normals[3 * uint32_t(idx.normal_index) + 2];
        }

        float tx = 0.0, ty = 0.0;
        if (idx.texcoord_index >= 0) {
          tx = attrib.texcoords[2 * uint32_t(idx.texcoord_index) + 0];
          ty = attrib.texcoords[2 * uint32_t(idx.texcoord_index) + 1];
        }

        Vertex vertex = {.positions = {vx, vy, vz},
                         .normals = {nx, ny, nz},
                         .textureCoordinates = {tx, ty},
                         .materialPropertiesIndex =
                             (int)materialPtr->getMaterialPropertiesCount() +
                             shapes[s].mesh.material_ids[f],
                         .modelIndex = (int)modelIndex};

        if (this->vertexMap.count(vertex) == 0) {
          this->vertexMap[vertex] = this->vertexList.size();
          this->vertexList.push_back(vertex);
        }

        this->indexList.push_back(this->vertexMap[vertex]);
      }
      index_offset += fv;
    }
  }

  std::map<std::string, int> textureNameIndexMap;
  textureNameIndexMap[""] = -1;
  for (uint32_t x = 0; x < materials.size(); x++) {
    if (textureNameIndexMap.find(materials[x].ambient_texname) ==
        textureNameIndexMap.end()) {

      textureNameIndexMap[materials[x].ambient_texname] =
          materialPtr->getTextureCount() + textureNameIndexMap.size() - 1;
    }
    if (textureNameIndexMap.find(materials[x].diffuse_texname) ==
        textureNameIndexMap.end()) {

      textureNameIndexMap[materials[x].diffuse_texname] =
          materialPtr->getTextureCount() + textureNameIndexMap.size() - 1;
    }
    if (textureNameIndexMap.find(materials[x].specular_texname) ==
        textureNameIndexMap.end()) {

      textureNameIndexMap[materials[x].specular_texname] =
          materialPtr->getTextureCount() + textureNameIndexMap.size() - 1;
    }

    Properties materialProperties = {
        .ambient = {materials[x].ambient[0], materials[x].ambient[1],
                    materials[x].ambient[2], 1},
        .diffuse = {materials[x].diffuse[0], materials[x].diffuse[1],
                    materials[x].diffuse[2], 1},
        .specular = {materials[x].specular[0], materials[x].specular[1],
                     materials[x].specular[2], 1},
        .transmittance = {materials[x].transmittance[0],
                          materials[x].transmittance[1],
                          materials[x].transmittance[2], 1},
        .emission = {materials[x].emission[0], materials[x].emission[1],
                     materials[x].emission[2], 1},
        .shininess = materials[x].shininess,
        .ior = materials[x].ior,
        .dissolve = materials[x].dissolve,
        .illum = materials[x].illum,

        .ambientTextureIndex =
            textureNameIndexMap[materials[x].ambient_texname],
        .diffuseTextureIndex =
            textureNameIndexMap[materials[x].diffuse_texname],
        .specularTextureIndex =
            textureNameIndexMap[materials[x].specular_texname]};

    this->materialPropertiesList.push_back(materialProperties);
  }

  void *hostMaterialPropertiesBuffer;
  this->materialPropertiesBufferPtr->mapMemory(&hostMaterialPropertiesBuffer, 0,
                                               32 * sizeof(Properties));
  for (uint32_t x = 0; x < this->materialPropertiesList.size(); x++) {
    VkDescriptorBufferInfo materialDescriptorBufferInfo = {
        .buffer = materialPropertiesBufferPtr->getBufferHandleRef(),
        .offset = (materialOffsetIndex + x) * sizeof(Properties),
        .range = sizeof(Properties)};

    memcpy(
        &((Properties *)hostMaterialPropertiesBuffer)[materialOffsetIndex + x],
        &this->materialPropertiesList[x], sizeof(Properties));

    materialPtr->getDescriptorSetGroupPtr()->updateDescriptorSets(
        {{1,
          3,
          materialOffsetIndex + x,
          1,
          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
          {},
          {materialDescriptorBufferInfo},
          {}}},
        {});
  }
  this->materialPropertiesBufferPtr->unmapMemory();

  materialPtr->incrementMaterialPropertiesCount(
      this->materialPropertiesList.size());

  for (auto pair : textureNameIndexMap) {
    if (pair.second != -1) {
      std::string texturePath = modelDirectory + "/" + pair.first;

      int texWidth, texHeight, texChannels;
      stbi_uc *pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight,
                                  &texChannels, STBI_rgb_alpha);
      VkDeviceSize imageSize = texWidth * texHeight * 4;

      if (!pixels) {
        std::cerr << "Could not load texture: " << texturePath << std::endl;
      }

      this->imagePtrList.push_back(std::unique_ptr<Image>(new Image(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          *enginePtr->getPhysicalDeviceHandlePtr().get(), 0, VK_IMAGE_TYPE_2D,
          VK_FORMAT_R8G8B8A8_SRGB, {(uint32_t)texWidth, (uint32_t)texHeight, 1},
          1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
          VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
          VK_IMAGE_LAYOUT_UNDEFINED, 0)));

      this->imageViewPtrList.push_back(std::shared_ptr<ImageView>(new ImageView(
          enginePtr->getDevicePtr()->getDeviceHandleRef(),
          this->imagePtrList[this->imagePtrList.size() - 1]
              ->getImageHandleRef(),
          0, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB,
          {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
           VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
          {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1})));

      std::shared_ptr<Buffer> imageBufferPtr =
          std::shared_ptr<Buffer>(new Buffer(
              enginePtr->getDevicePtr()->getDeviceHandleRef(),
              *enginePtr->getPhysicalDeviceHandlePtr().get(), 0, imageSize,
              VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
              VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

      void *hostImageBuffer;
      imageBufferPtr->mapMemory(&hostImageBuffer, 0, imageSize);
      memcpy(hostImageBuffer, pixels, imageSize);
      imageBufferPtr->unmapMemory();

      std::shared_ptr<Fence> fencePtr = std::shared_ptr<Fence>(
          new Fence(enginePtr->getDevicePtr()->getDeviceHandleRef(),
                    (VkFenceCreateFlagBits)0));

      enginePtr->getUtilityCommandBufferGroupPtr()->beginRecording(
          0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

      enginePtr->getUtilityCommandBufferGroupPtr()->createPipelineBarrierCmd(
          0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          VK_PIPELINE_STAGE_TRANSFER_BIT, 0, {}, {},
          {{VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            enginePtr->getQueueFamilyIndex(),
            enginePtr->getQueueFamilyIndex(),
            this->imagePtrList[this->imagePtrList.size() - 1]
                ->getImageHandleRef(),
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

      enginePtr->getUtilityCommandBufferGroupPtr()->endRecording(0);

      enginePtr->getUtilityCommandBufferGroupPtr()->submit(
          enginePtr->getDevicePtr()->getQueueHandleRef(
              enginePtr->getQueueFamilyIndex(), 0),
          {{{}, {}, {0}, {}}}, fencePtr->getFenceHandleRef());

      fencePtr->waitForSignal(UINT32_MAX);
      fencePtr->reset();

      enginePtr->getUtilityCommandBufferGroupPtr()->beginRecording(
          0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

      this->imagePtrList[this->imagePtrList.size() - 1]->copyFromBufferCmd(
          enginePtr->getUtilityCommandBufferGroupPtr()
              ->getCommandBufferHandleRef(0),
          imageBufferPtr->getBufferHandleRef(),
          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          {{0,
            0,
            0,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
            {0, 0, 0},
            {(uint32_t)texWidth, (uint32_t)texHeight, 1}}});

      enginePtr->getUtilityCommandBufferGroupPtr()->endRecording(0);

      enginePtr->getUtilityCommandBufferGroupPtr()->submit(
          enginePtr->getDevicePtr()->getQueueHandleRef(
              enginePtr->getQueueFamilyIndex(), 0),
          {{{}, {}, {0}, {}}}, fencePtr->getFenceHandleRef());

      fencePtr->waitForSignal(UINT32_MAX);
      fencePtr->reset();

      enginePtr->getUtilityCommandBufferGroupPtr()->beginRecording(
          0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

      enginePtr->getUtilityCommandBufferGroupPtr()->createPipelineBarrierCmd(
          0, VK_PIPELINE_STAGE_TRANSFER_BIT,
          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, {}, {},
          {{VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            enginePtr->getQueueFamilyIndex(),
            enginePtr->getQueueFamilyIndex(),
            this->imagePtrList[this->imagePtrList.size() - 1]
                ->getImageHandleRef(),
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

      enginePtr->getUtilityCommandBufferGroupPtr()->endRecording(0);

      enginePtr->getUtilityCommandBufferGroupPtr()->submit(
          enginePtr->getDevicePtr()->getQueueHandleRef(
              enginePtr->getQueueFamilyIndex(), 0),
          {{{}, {}, {0}, {}}}, fencePtr->getFenceHandleRef());

      fencePtr->waitForSignal(UINT32_MAX);
      fencePtr->reset();
    }
  }

  for (uint32_t x = 0; x < this->imageViewPtrList.size(); x++) {
    VkDescriptorImageInfo descriptorImageInfo = {
        .sampler = VK_NULL_HANDLE,
        .imageView = this->imageViewPtrList[x]->getImageViewHandleRef(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    materialPtr->getDescriptorSetGroupPtr()->updateDescriptorSets(
        {{1,
          5,
          textureOffsetIndex + x,
          1,
          VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
          {descriptorImageInfo},
          {},
          {}}},
        {});
  }

  materialPtr->incrementTextureCount(this->imageViewPtrList.size());

  VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
      .pNext = NULL,
      .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
      .deviceMask = 0};

  VkBufferUsageFlagBits extraUsageFlagBits = (VkBufferUsageFlagBits)0;
  if (materialPtr->getMaterialType() == Material::MaterialType::RAY_TRACE) {
    extraUsageFlagBits =
        (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
  }

  this->vertexBufferPtr = std::shared_ptr<Buffer>(new Buffer(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
      sizeof(Vertex) * this->vertexList.size(),
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | extraUsageFlagBits,
      VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {&memoryAllocateFlagsInfo}));

  void *hostVertexBuffer;
  this->vertexBufferPtr->mapMemory(&hostVertexBuffer, 0,
                                   sizeof(Vertex) * this->vertexList.size());
  memcpy(hostVertexBuffer, this->vertexList.data(),
         sizeof(Vertex) * this->vertexList.size());
  this->vertexBufferPtr->unmapMemory();

  this->indexBufferPtr = std::shared_ptr<Buffer>(new Buffer(
      enginePtr->getDevicePtr()->getDeviceHandleRef(),
      *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
      sizeof(uint32_t) * this->indexList.size(),
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | extraUsageFlagBits,
      VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {&memoryAllocateFlagsInfo}));

  void *hostIndexBuffer;
  this->indexBufferPtr->mapMemory(&hostIndexBuffer, 0,
                                  sizeof(uint32_t) * this->indexList.size());
  memcpy(hostIndexBuffer, this->indexList.data(),
         sizeof(uint32_t) * this->indexList.size());
  this->indexBufferPtr->unmapMemory();
}

Model::~Model() {}

void Model::render(
    std::shared_ptr<CommandBufferGroup::CommandBufferInheritanceInfoParam>
        commandBufferInheritanceInfoParamPtr,
    uint32_t commandBufferIndex) {

  VkCommandBufferUsageFlagBits commandBufferUsageFlagBits =
      (VkCommandBufferUsageFlagBits)0;

  if (commandBufferInheritanceInfoParamPtr->renderPassHandle !=
      VK_NULL_HANDLE) {

    commandBufferUsageFlagBits =
        VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
  }

  this->enginePtr->getSecondaryCommandBufferGroupPtr()->beginRecording(
      commandBufferIndex, commandBufferUsageFlagBits,
      commandBufferInheritanceInfoParamPtr);

  this->materialPtr->render(this->enginePtr->getSecondaryCommandBufferGroupPtr()
                                ->getCommandBufferHandleRef(commandBufferIndex),
                            shared_from_this());

  this->enginePtr->getSecondaryCommandBufferGroupPtr()->endRecording(
      commandBufferIndex);
}

void Model::setPosition(float x, float y, float z) {
  this->position[0] = x;
  this->position[1] = y;
  this->position[2] = z;

  this->updateModelMatrix();
}

void Model::updatePosition(float x, float y, float z) {
  this->position[0] += x;
  this->position[1] += y;
  this->position[2] += z;

  this->updateModelMatrix();
}

void Model::setRotation(float yaw, float pitch, float roll) {
  this->yaw = yaw;
  this->pitch = pitch;
  this->roll = roll;

  this->updateModelMatrix();
}

void Model::updateRotation(float yaw, float pitch, float roll) {
  this->yaw += yaw;
  this->pitch += pitch;
  this->roll += roll;

  this->updateModelMatrix();
}

std::shared_ptr<VkDescriptorBufferInfo>
Model::getModelDescriptorBufferInfoPtr() {
  return this->modelDescriptorBufferInfoPtr;
}

uint32_t Model::getModelIndex() { return this->modelIndex; }

bool Model::getIsModelBufferDirty() { return this->isModelBufferDirty; }

void Model::resetIsModelBufferDirty() { this->isModelBufferDirty = false; }

void Model::updateModelMatrix() {
  this->modelShaderStructure.modelMatrix[0] =
      cosf(this->pitch) * cosf(this->yaw);
  this->modelShaderStructure.modelMatrix[1] =
      sinf(this->pitch) * cosf(this->yaw);
  this->modelShaderStructure.modelMatrix[2] = -sinf(this->yaw);
  this->modelShaderStructure.modelMatrix[3] = 0.0;

  this->modelShaderStructure.modelMatrix[4] =
      cosf(this->pitch) * sinf(this->yaw) * sinf(this->roll) -
      sinf(this->pitch) * cosf(roll);
  this->modelShaderStructure.modelMatrix[5] =
      sinf(this->pitch) * sinf(this->yaw) * sinf(this->roll) +
      cosf(this->pitch) * cosf(roll);
  this->modelShaderStructure.modelMatrix[6] =
      cosf(this->yaw) * sinf(this->roll);
  this->modelShaderStructure.modelMatrix[7] = 0.0;

  this->modelShaderStructure.modelMatrix[8] =
      cosf(this->pitch) * sinf(this->yaw) * cosf(this->roll) +
      sinf(this->pitch) * sinf(roll);
  this->modelShaderStructure.modelMatrix[9] =
      sinf(this->pitch) * sinf(this->yaw) * cosf(this->roll) -
      cosf(this->pitch) * sinf(roll);
  this->modelShaderStructure.modelMatrix[10] =
      cosf(this->yaw) * cosf(this->roll);
  this->modelShaderStructure.modelMatrix[11] = 0.0;

  this->modelShaderStructure.modelMatrix[12] = this->position[0];
  this->modelShaderStructure.modelMatrix[13] = this->position[1];
  this->modelShaderStructure.modelMatrix[14] = this->position[2];
  this->modelShaderStructure.modelMatrix[15] = 1.0;

  void *hostModelsBuffer;
  this->modelsBufferPtr->mapMemory(&hostModelsBuffer, 0,
                                   32 * sizeof(ModelShaderStructure));
  memcpy(&((ModelShaderStructure *)hostModelsBuffer)[modelIndex],
         &this->modelShaderStructure, sizeof(ModelShaderStructure));
  this->modelsBufferPtr->unmapMemory();

  this->isModelBufferDirty = true;
}

std::shared_ptr<Buffer> Model::getVertexBufferPtr() {
  return this->vertexBufferPtr;
}

uint32_t Model::getVertexCount() { return this->vertexList.size(); }

std::shared_ptr<Buffer> Model::getIndexBufferPtr() {
  return this->indexBufferPtr;
}

uint32_t Model::getIndexCount() { return this->indexList.size(); }

std::shared_ptr<Material> Model::getMaterialPtr() { return this->materialPtr; }
