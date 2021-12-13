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
                         .textureCoordinates = {tx, ty}};

        if (this->vertexMap.count(vertex) == 0) {
          this->vertexMap[vertex] = this->vertexList.size();
          this->vertexList.push_back(vertex);
        }

        this->indexList.push_back(this->vertexMap[vertex]);
      }
      index_offset += fv;
    }
  }

  for (uint32_t x = 0; x < this->vertexList.size(); x++) {
    std::cout << this->vertexList[x].positions[0] << std::endl;
  }

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
