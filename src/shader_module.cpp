#include "vrk/shader_module.h"

ShaderModule::ShaderModule(VkDevice* deviceHandlePtr,
    std::vector<uint32_t> code) : Component("shader module") {

  this->shaderModuleHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;

  this->code = code;
}

ShaderModule::~ShaderModule() {
  vkDestroyShaderModule(*this->deviceHandlePtr, this->shaderModuleHandle, NULL);
}

bool ShaderModule::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkShaderModuleCreateInfo shaderModuleCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .codeSize = (uint32_t)this->code.size() * sizeof(uint32_t),
    .pCode = this->code.data()
  };

  VkResult result = vkCreateShaderModule(*this->deviceHandlePtr,
      &shaderModuleCreateInfo, NULL, &this->shaderModuleHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateShaderModule");
  }

  return true;
}