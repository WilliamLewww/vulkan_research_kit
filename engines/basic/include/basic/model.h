#pragma once
#include "basic/material.h"

#include <vrk/buffer.h>
#include <vrk/command_buffer_group.h>
#include <vrk/device.h>

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
  };

  Model(std::shared_ptr<Engine> enginePtr, std::shared_ptr<Scene> scenePtr,
        std::string modelName, std::string modelPath,
        std::shared_ptr<Material> materialPtr);

  ~Model();

  void
  render(std::shared_ptr<CommandBufferGroup::CommandBufferInheritanceInfoParam>
             commandBufferInheritanceInfoParamPtr,
         uint32_t commandBufferIndex);

private:
  struct CompareVertex {
    bool operator()(const Vertex &a, const Vertex &b) const {
      if (std::memcmp(a.positions, b.positions, sizeof(float) * 3) == 0 &&
          std::memcmp(a.normals, b.normals, sizeof(float) * 3) == 0) {

        return std::memcmp(a.textureCoordinates, b.textureCoordinates,
                           sizeof(float) * 2);
      }

      if (std::memcmp(a.positions, b.positions, sizeof(float) * 3) == 0) {
        return std::memcmp(a.normals, b.normals, sizeof(float) * 3);
      }

      return std::memcmp(a.positions, b.positions, sizeof(float) * 3);
    }
  };

  std::shared_ptr<Engine> enginePtr;

  std::shared_ptr<Scene> scenePtr;

  std::string modelName;

  std::map<Vertex, uint32_t, CompareVertex> vertexMap;

  std::vector<Vertex> vertexList;

  std::vector<uint32_t> indexList;

  std::shared_ptr<Material> materialPtr;

  std::unique_ptr<Buffer> vertexBufferPtr;

  std::unique_ptr<Buffer> indexBufferPtr;
};
