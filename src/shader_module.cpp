#include "vrk/shader_module.h"

ShaderModule::ShaderModule(VkDevice &deviceHandleRef,
                           std::vector<uint32_t> code)
    : deviceHandleRef(deviceHandleRef) {

  this->shaderModuleHandle = VK_NULL_HANDLE;

  VkShaderModuleCreateInfo shaderModuleCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .codeSize = (uint32_t)code.size() * sizeof(uint32_t),
      .pCode = code.data()};

  VkResult result =
      vkCreateShaderModule(deviceHandleRef, &shaderModuleCreateInfo, NULL,
                           &this->shaderModuleHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateShaderModule");
  }
}

ShaderModule::~ShaderModule() {
  vkDestroyShaderModule(this->deviceHandleRef, this->shaderModuleHandle, NULL);
}

VkShaderModule &ShaderModule::getShaderModuleHandleRef() {
  return this->shaderModuleHandle;
}