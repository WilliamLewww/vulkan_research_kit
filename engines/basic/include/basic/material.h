#pragma once

#include <vrk/device.h>
#include <vrk/resource.h>
#include <vrk/shader_module.h>

#include <memory>

class Material {
public:
  Material(std::shared_ptr<Device> devicePtr, std::string vertexFileName,
           std::string fragmentFileName);

  ~Material();

private:
  std::unique_ptr<ShaderModule> vertexShaderModulePtr;

  std::unique_ptr<ShaderModule> fragmentShaderModulePtr;
};
