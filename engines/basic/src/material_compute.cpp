#include "basic/material_compute.h"
#include "basic/engine.h"
#include "basic/scene.h"

MaterialCompute::MaterialCompute(
    std::shared_ptr<Engine> enginePtr, std::string materialName,
    std::map<ShaderStage, std::string> shaderStageNameMap)
    : Material(enginePtr, materialName, shaderStageNameMap,
               Material::MaterialType::COMPUTE) {

  if (shaderStageModuleMap.begin()->first == ShaderStage::COMPUTE) {
    ComputePipelineGroup::PipelineShaderStageCreateInfoParam
        pipelineShaderStageCreateInfo = {
            .pipelineShaderStageCreateFlags = 0,
            .shaderStageFlagBits = VK_SHADER_STAGE_COMPUTE_BIT,
            .shaderModuleHandleRef = shaderStageModuleMap.begin()
                                         ->second->getShaderModuleHandleRef(),
            .entryPointName = "main",
            .specializationInfoPtr = NULL};

    this->computeDescriptorSetLayoutPtr =
        std::shared_ptr<DescriptorSetLayout>(new DescriptorSetLayout(
            enginePtr->getDevicePtr()->getDeviceHandleRef(), 0, {}));

    this->initializeDescriptors(this->COMPUTE_MATERIAL_DESCRIPTOR_COUNTS,
                                this->computeDescriptorSetLayoutPtr);

    this->computePipelineGroupPtr =
        std::unique_ptr<ComputePipelineGroup>(new ComputePipelineGroup(
            enginePtr->getDevicePtr()->getDeviceHandleRef(),
            {{0, pipelineShaderStageCreateInfo,
              this->pipelineLayoutPtr->getPipelineLayoutHandleRef(),
              VK_NULL_HANDLE, 0}}));
  }
}

MaterialCompute::~MaterialCompute() {}

void MaterialCompute::render(VkCommandBuffer commandBufferHandle,
                             std::shared_ptr<Model> modelPtr) {

  this->computePipelineGroupPtr->bindPipelineCmd(0, commandBufferHandle);

  this->descriptorSetGroupPtr->bindDescriptorSetsCmd(
      commandBufferHandle, VK_PIPELINE_BIND_POINT_COMPUTE,
      this->pipelineLayoutPtr->getPipelineLayoutHandleRef(), 0, {0, 1}, {});

  this->computePipelineGroupPtr->dispatchCmd(commandBufferHandle, 800, 600, 1);
}
