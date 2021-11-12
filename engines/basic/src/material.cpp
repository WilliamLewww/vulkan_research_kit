#include "basic/material.h"

Material::Material(std::shared_ptr<Device> devicePtr, std::string name,
                   std::string vertexFileName, std::string fragmentFileName)
    : name(name) {

  std::ifstream vertexFile(Resource::findResource(vertexFileName),
                           std::ios::binary | std::ios::ate);
  std::streamsize vertexFileSize = vertexFile.tellg();
  vertexFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> vertexShaderSource(vertexFileSize / sizeof(uint32_t));
  vertexFile.read((char *)vertexShaderSource.data(), vertexFileSize);
  vertexFile.close();

  this->vertexShaderModulePtr = std::unique_ptr<ShaderModule>(
      new ShaderModule(devicePtr->getDeviceHandleRef(), vertexShaderSource));

  std::ifstream fragmentFile(Resource::findResource(fragmentFileName),
                             std::ios::binary | std::ios::ate);
  std::streamsize fragmentFileSize = fragmentFile.tellg();
  fragmentFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> fragmentShaderSource(fragmentFileSize /
                                             sizeof(uint32_t));
  fragmentFile.read((char *)fragmentShaderSource.data(), fragmentFileSize);
  fragmentFile.close();

  this->fragmentShaderModulePtr = std::unique_ptr<ShaderModule>(
      new ShaderModule(devicePtr->getDeviceHandleRef(), fragmentShaderSource));
}

Material::~Material() {}
