#include "basic/model.h"
#include "basic/engine.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Model::Model(std::shared_ptr<Engine> enginePtr, std::shared_ptr<Scene> scenePtr,
             std::string modelName, std::string modelPath,
             std::shared_ptr<Material> materialPtr)
    : enginePtr(enginePtr), scenePtr(scenePtr), modelName(modelName),
      materialPtr(materialPtr) {

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

        float nx = attrib.normals[3 * uint32_t(idx.normal_index) + 0];
        float ny = attrib.normals[3 * uint32_t(idx.normal_index) + 1];
        float nz = attrib.normals[3 * uint32_t(idx.normal_index) + 2];

        float tx = attrib.texcoords[2 * uint32_t(idx.texcoord_index) + 0];
        float ty = attrib.texcoords[2 * uint32_t(idx.texcoord_index) + 1];

        Vertex vertex = {.positions = {vx, vy, vz},
                         .normals = {nx, ny, nz},
                         .textureCoordinates = {tx, ty},
                         .materialPropertiesIndex =
                             (int)materialPtr->getMaterialPropertiesCount() +
                             shapes[s].mesh.material_ids[f]};

        if (this->vertexMap.count(vertex) == 0) {
          this->vertexMap[vertex] = this->vertexList.size();
          this->vertexList.push_back(vertex);
        }

        this->indexList.push_back(this->vertexMap[vertex]);
      }
      index_offset += fv;
    }
  }

  std::map<std::string, int> textureIndexMap;
  textureIndexMap[""] = -1;
  for (uint32_t x = 0; x < materials.size(); x++) {
    if (textureIndexMap.find(materials[x].ambient_texname) ==
        textureIndexMap.end()) {

      textureIndexMap[materials[x].ambient_texname] = textureIndexMap.size();
    }
    if (textureIndexMap.find(materials[x].diffuse_texname) ==
        textureIndexMap.end()) {

      textureIndexMap[materials[x].diffuse_texname] = textureIndexMap.size();
    }
    if (textureIndexMap.find(materials[x].specular_texname) ==
        textureIndexMap.end()) {

      textureIndexMap[materials[x].specular_texname] = textureIndexMap.size();
    }

    Material::Properties materialProperties = {
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

        .ambientTextureIndex = textureIndexMap[materials[x].ambient_texname],
        .diffuseTextureIndex = textureIndexMap[materials[x].diffuse_texname],
        .specularTextureIndex = textureIndexMap[materials[x].specular_texname]};

    this->materialPropertiesList.push_back(materialProperties);
  }

  materialPtr->appendMaterialPropertiesDescriptors(
      this->materialPropertiesList);

  // TODO: import textures using stb_image
  for (auto pair : textureIndexMap) {
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
          enginePtr->devicePtr->getDeviceHandleRef(),
          *enginePtr->physicalDeviceHandlePtr.get(), 0, VK_IMAGE_TYPE_2D,
          VK_FORMAT_R8G8B8A8_SRGB, {(uint32_t)texWidth, (uint32_t)texHeight, 1},
          1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
          VK_SHARING_MODE_EXCLUSIVE, {enginePtr->queueFamilyIndex},
          VK_IMAGE_LAYOUT_UNDEFINED, 0)));

      this->imageViewPtrList.push_back(std::shared_ptr<ImageView>(new ImageView(
          enginePtr->devicePtr->getDeviceHandleRef(),
          this->imagePtrList[this->imagePtrList.size() - 1]
              ->getImageHandleRef(),
          0, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB,
          {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
           VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
          {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1})));

      std::shared_ptr<Buffer> imageBufferPtr = std::shared_ptr<Buffer>(
          new Buffer(enginePtr->devicePtr->getDeviceHandleRef(),
                     *enginePtr->physicalDeviceHandlePtr.get(), 0, imageSize,
                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_SHARING_MODE_EXCLUSIVE, {enginePtr->queueFamilyIndex},
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

      void *hostImageBuffer;
      imageBufferPtr->mapMemory(&hostImageBuffer, 0, imageSize);
      memcpy(hostImageBuffer, pixels, imageSize);
      imageBufferPtr->unmapMemory();

      std::shared_ptr<Fence> fencePtr = std::shared_ptr<Fence>(
          new Fence(enginePtr->devicePtr->getDeviceHandleRef(),
                    (VkFenceCreateFlagBits)0));

      enginePtr->utilityCommandBufferGroupPtr->beginRecording(
          0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

      enginePtr->utilityCommandBufferGroupPtr->createPipelineBarrierCmd(
          0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          VK_PIPELINE_STAGE_TRANSFER_BIT, 0, {}, {},
          {{VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            enginePtr->queueFamilyIndex,
            enginePtr->queueFamilyIndex,
            this->imagePtrList[this->imagePtrList.size() - 1]
                ->getImageHandleRef(),
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

      enginePtr->utilityCommandBufferGroupPtr->endRecording(0);

      enginePtr->utilityCommandBufferGroupPtr->submit(
          enginePtr->devicePtr->getQueueHandleRef(enginePtr->queueFamilyIndex,
                                                  0),
          {{{}, {}, {0}, {}}}, fencePtr->getFenceHandleRef());

      fencePtr->waitForSignal(UINT32_MAX);
      fencePtr->reset();

      enginePtr->utilityCommandBufferGroupPtr->beginRecording(
          0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

      this->imagePtrList[this->imagePtrList.size() - 1]->copyFromBufferCmd(
          enginePtr->utilityCommandBufferGroupPtr->getCommandBufferHandleRef(0),
          imageBufferPtr->getBufferHandleRef(),
          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          {{0,
            0,
            0,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
            {0, 0, 0},
            {250, 250, 1}}});

      enginePtr->utilityCommandBufferGroupPtr->endRecording(0);

      enginePtr->utilityCommandBufferGroupPtr->submit(
          enginePtr->devicePtr->getQueueHandleRef(enginePtr->queueFamilyIndex,
                                                  0),
          {{{}, {}, {0}, {}}}, fencePtr->getFenceHandleRef());

      fencePtr->waitForSignal(UINT32_MAX);
      fencePtr->reset();

      enginePtr->utilityCommandBufferGroupPtr->beginRecording(
          0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

      enginePtr->utilityCommandBufferGroupPtr->createPipelineBarrierCmd(
          0, VK_PIPELINE_STAGE_TRANSFER_BIT,
          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, {}, {},
          {{VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            enginePtr->queueFamilyIndex,
            enginePtr->queueFamilyIndex,
            this->imagePtrList[this->imagePtrList.size() - 1]
                ->getImageHandleRef(),
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

      enginePtr->utilityCommandBufferGroupPtr->endRecording(0);

      enginePtr->utilityCommandBufferGroupPtr->submit(
          enginePtr->devicePtr->getQueueHandleRef(enginePtr->queueFamilyIndex,
                                                  0),
          {{{}, {}, {0}, {}}}, fencePtr->getFenceHandleRef());

      fencePtr->waitForSignal(UINT32_MAX);
      fencePtr->reset();
    }
  }

  materialPtr->appendTextureDescriptors(this->imageViewPtrList);

  this->vertexBufferPtr = std::unique_ptr<Buffer>(new Buffer(
      enginePtr->devicePtr->getDeviceHandleRef(),
      *enginePtr->physicalDeviceHandlePtr.get(), 0,
      sizeof(Vertex) * this->vertexList.size(),
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      {enginePtr->queueFamilyIndex}, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  void *hostVertexBuffer;
  this->vertexBufferPtr->mapMemory(&hostVertexBuffer, 0,
                                   sizeof(Vertex) * this->vertexList.size());
  memcpy(hostVertexBuffer, this->vertexList.data(),
         sizeof(Vertex) * this->vertexList.size());
  this->vertexBufferPtr->unmapMemory();

  this->indexBufferPtr = std::unique_ptr<Buffer>(new Buffer(
      enginePtr->devicePtr->getDeviceHandleRef(),
      *enginePtr->physicalDeviceHandlePtr.get(), 0,
      sizeof(uint32_t) * this->indexList.size(),
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      {enginePtr->queueFamilyIndex}, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

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
  this->enginePtr->secondaryCommandBufferGroupPtr->beginRecording(
      commandBufferIndex, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
      commandBufferInheritanceInfoParamPtr);

  this->materialPtr->graphicsPipelineGroupPtr->bindPipelineCmd(
      0, this->enginePtr->secondaryCommandBufferGroupPtr
             ->getCommandBufferHandleRef(commandBufferIndex));

  this->vertexBufferPtr->bindVertexBufferCmd(
      this->enginePtr->secondaryCommandBufferGroupPtr
          ->getCommandBufferHandleRef(commandBufferIndex),
      0);

  this->indexBufferPtr->bindIndexBufferCmd(
      this->enginePtr->secondaryCommandBufferGroupPtr
          ->getCommandBufferHandleRef(commandBufferIndex),
      VK_INDEX_TYPE_UINT32);

  this->materialPtr->descriptorSetGroupPtr->bindDescriptorSetsCmd(
      this->enginePtr->secondaryCommandBufferGroupPtr
          ->getCommandBufferHandleRef(commandBufferIndex),
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      this->materialPtr->pipelineLayoutPtr->getPipelineLayoutHandleRef(), 0,
      {0}, {});

  this->materialPtr->graphicsPipelineGroupPtr->drawIndexedCmd(
      this->enginePtr->secondaryCommandBufferGroupPtr
          ->getCommandBufferHandleRef(commandBufferIndex),
      this->indexList.size(), 1, 0, 0, 0);

  this->enginePtr->secondaryCommandBufferGroupPtr->endRecording(
      commandBufferIndex);
}
