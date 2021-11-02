#pragma once

#include <vrk/resource.h>
#include <vrk/shader_module.h>

#include <memory>

class Material {
public:
  Material(std::string vertexFileName, std::string fragmentFileName);

  ~Material();

private:
  std::unique_ptr<ShaderModule> vertexShaderModulePtr;

  std::unique_ptr<ShaderModule> fragmentShaderModulePtr;
};
