#pragma once
#include "basic/material.h"

#include <vrk/buffer.h>
#include <vrk/command_buffer_group.h>
#include <vrk/device.h>
#include <vrk/image.h>
#include <vrk/image_view.h>

#include <map>
#include <string>

class Engine;
class Scene;

class Model : public std::enable_shared_from_this<Model> {
public:
  struct Vertex {
    float positions[3];
    float normals[3];
    float textureCoordinates[2];
    int materialPropertiesIndex;
    int modelIndex;
  };

  struct ModelShaderStructure {
    alignas(16) float modelMatrix[16];
  };

  Model(std::shared_ptr<Engine> enginePtr, std::shared_ptr<Scene> scenePtr,
        std::string modelName, std::string modelPath,
        std::shared_ptr<Material> materialPtr, uint32_t modelIndex,
        std::shared_ptr<Buffer> modelsBufferPtr);

  ~Model();

  void setPosition(float x, float y, float z);

  void updatePosition(float x, float y, float z);

  void setRotation(float yaw, float pitch, float roll);

  void updateRotation(float yaw, float pitch, float roll);

  void
  render(std::shared_ptr<CommandBufferGroup::CommandBufferInheritanceInfoParam>
             commandBufferInheritanceInfoParamPtr,
         uint32_t commandBufferIndex);

  uint32_t getModelIndex();

  bool getIsModelBufferDirty();

  void resetIsModelBufferDirty();

  std::shared_ptr<VkDescriptorBufferInfo> getModelDescriptorBufferInfoPtr();

  std::shared_ptr<Buffer> getVertexBufferPtr();

  std::shared_ptr<Buffer> getIndexBufferPtr();

  uint32_t getIndexCount();

private:
  struct CompareVertex {
    bool operator()(const Vertex &a, const Vertex &b) const {
      if (std::memcmp(a.positions, b.positions, sizeof(float) * 3) == 0 &&
          std::memcmp(a.normals, b.normals, sizeof(float) * 3) == 0) {

        return std::memcmp(a.textureCoordinates, b.textureCoordinates,
                           sizeof(float) * 2) < 0;
      }

      if (std::memcmp(a.positions, b.positions, sizeof(float) * 3) == 0) {
        return std::memcmp(a.normals, b.normals, sizeof(float) * 3) < 0;
      }

      return std::memcmp(a.positions, b.positions, sizeof(float) * 3) < 0;
    }
  };

  void updateModelMatrix();

  std::shared_ptr<Engine> enginePtr;

  std::shared_ptr<Scene> scenePtr;

  std::string modelName;

  std::map<Vertex, uint32_t, CompareVertex> vertexMap;

  std::vector<Vertex> vertexList;

  std::vector<uint32_t> indexList;

  std::shared_ptr<Material> materialPtr;

  std::vector<Material::Properties> materialPropertiesList;

  std::shared_ptr<Buffer> vertexBufferPtr;

  std::shared_ptr<Buffer> indexBufferPtr;

  std::vector<std::unique_ptr<Image>> imagePtrList;

  std::vector<std::shared_ptr<ImageView>> imageViewPtrList;

  std::shared_ptr<Buffer> modelsBufferPtr;

  uint32_t modelIndex;

  ModelShaderStructure modelShaderStructure;

  std::shared_ptr<VkDescriptorBufferInfo> modelDescriptorBufferInfoPtr;

  bool isModelBufferDirty;

  float position[3];

  float yaw;

  float pitch;

  float roll;
};
