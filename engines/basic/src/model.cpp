#include "basic/model.h"
#include "basic/engine.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

Model::Model(std::shared_ptr<Engine> enginePtr, std::string modelName,
             std::string modelPath, std::shared_ptr<Material> materialPtr)
    : enginePtr(enginePtr), modelName(modelName), materialPtr(materialPtr) {

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

void Model::render(uint32_t frameIndex) {
  this->enginePtr->commandBufferGroupPtr->beginRecording(
      0, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

  this->enginePtr->renderPassPtr->beginRenderPassCmd(
      this->enginePtr->commandBufferGroupPtr->getCommandBufferHandleRef(0),
      this->enginePtr->framebufferPtrList[frameIndex]
          ->getFramebufferHandleRef(),
      {{0, 0}, {800, 600}}, {{{0.0, 0.0, 0.0, 1.0}}},
      VK_SUBPASS_CONTENTS_INLINE);

  this->materialPtr->graphicsPipelineGroupPtr->bindPipelineCmd(
      0, this->enginePtr->commandBufferGroupPtr->getCommandBufferHandleRef(0));

  this->vertexBufferPtr->bindVertexBufferCmd(
      this->enginePtr->commandBufferGroupPtr->getCommandBufferHandleRef(0), 0);
  this->indexBufferPtr->bindIndexBufferCmd(
      this->enginePtr->commandBufferGroupPtr->getCommandBufferHandleRef(0),
      VK_INDEX_TYPE_UINT32);

  this->materialPtr->graphicsPipelineGroupPtr->drawIndexedCmd(
      this->enginePtr->commandBufferGroupPtr->getCommandBufferHandleRef(0),
      this->totalIndexCount, 1, 0, 0, 0);

  this->enginePtr->renderPassPtr->endRenderPassCmd(
      this->enginePtr->commandBufferGroupPtr->getCommandBufferHandleRef(0));
}
