#include "basic/model.h"

Model::Model(std::string name, std::string modelPath,
             std::shared_ptr<Material> materialPtr)
    : name(name), materialPtr(materialPtr) {}

Model::~Model() {}
