#include "basic/model.h"
#include "basic/engine.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

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

  this->vertexBufferPtr = std::unique_ptr<Buffer>(new Buffer(
      enginePtr->devicePtr->getDeviceHandleRef(),
      *enginePtr->physicalDeviceHandlePtr.get(), 0,
      sizeof(float) * attrib.vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {enginePtr->queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  void *hostVertexBuffer;
  this->vertexBufferPtr->mapMemory(&hostVertexBuffer, 0,
                                   sizeof(float) * attrib.vertices.size());
  memcpy(hostVertexBuffer, attrib.vertices.data(),
         sizeof(float) * attrib.vertices.size());
  this->vertexBufferPtr->unmapMemory();

  this->totalIndexCount = 0;
  for (uint32_t x = 0; x < shapes.size(); x++) {
    this->totalIndexCount += shapes[x].mesh.indices.size();
  }
  this->indexBufferPtr = std::unique_ptr<Buffer>(new Buffer(
      enginePtr->devicePtr->getDeviceHandleRef(),
      *enginePtr->physicalDeviceHandlePtr.get(), 0,
      sizeof(uint32_t) * this->totalIndexCount,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      {enginePtr->queueFamilyIndex}, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

  void *hostIndexBuffer;
  this->indexBufferPtr->mapMemory(&hostIndexBuffer, 0,
                                  sizeof(uint32_t) * this->totalIndexCount);
  uint32_t currentIndexOffset = 0;
  for (uint32_t x = 0; x < shapes.size(); x++) {
    for (uint32_t y = 0; y < shapes[x].mesh.indices.size(); y++) {
      ((uint32_t *)hostIndexBuffer)[currentIndexOffset + y] =
          shapes[x].mesh.indices[y].vertex_index;
    }
    currentIndexOffset += shapes[x].mesh.indices.size();
  }
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
      this->totalIndexCount, 1, 0, 0, 0);

  this->enginePtr->secondaryCommandBufferGroupPtr->endRecording(
      commandBufferIndex);
}
