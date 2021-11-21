#include "basic/model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

Model::Model(std::shared_ptr<Device> devicePtr, std::string modelName,
             std::string modelPath, std::shared_ptr<Material> materialPtr)
    : modelName(modelName), materialPtr(materialPtr) {

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
}

Model::~Model() {}
