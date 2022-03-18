#include "basic/material_compute.h"
#include "basic/engine.h"
#include "basic/scene.h"

MaterialCompute::MaterialCompute(
    std::shared_ptr<Engine> enginePtr, std::string materialName,
    std::map<ShaderStage, std::string> shaderStageNameMap)
    : Material(enginePtr, materialName, shaderStageNameMap,
               Material::MaterialType::COMPUTE) {

  for (uint32_t x = 0; x < enginePtr->getSwapchainImageCount(); x++) {
    this->imagePtrMap["output" + x] = std::unique_ptr<Image>(
        new Image(enginePtr->getDevicePtr()->getDeviceHandleRef(),
                  *enginePtr->getPhysicalDeviceHandlePtr().get(), 0,
                  VK_IMAGE_TYPE_2D, enginePtr->getSurfaceFormatList()[0].format,
                  {enginePtr->getSurfaceCapabilities().currentExtent.width,
                   enginePtr->getSurfaceCapabilities().currentExtent.height, 1},
                  1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
                  VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                  VK_SHARING_MODE_EXCLUSIVE, {enginePtr->getQueueFamilyIndex()},
                  VK_IMAGE_LAYOUT_UNDEFINED, 0));

    this->imageViewPtrMap["output" + x] =
        std::unique_ptr<ImageView>(new ImageView(
            enginePtr->getDevicePtr()->getDeviceHandleRef(),
            this->imagePtrMap["output" + x]->getImageHandleRef(), 0,
            VK_IMAGE_VIEW_TYPE_2D, enginePtr->getSurfaceFormatList()[0].format,
            {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
             VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}));
  }

  if (shaderStageModulePtrMap.begin()->first == ShaderStage::COMPUTE) {
    ComputePipelineGroup::PipelineShaderStageCreateInfoParam
        pipelineShaderStageCreateInfo = {
            .pipelineShaderStageCreateFlags = 0,
            .shaderStageFlagBits = VK_SHADER_STAGE_COMPUTE_BIT,
            .shaderModuleHandleRef = shaderStageModulePtrMap.begin()
                                         ->second->getShaderModuleHandleRef(),
            .entryPointName = "main",
            .specializationInfoPtr = NULL};

    this->computeDescriptorSetLayoutPtr =
        std::shared_ptr<DescriptorSetLayout>(new DescriptorSetLayout(
            enginePtr->getDevicePtr()->getDeviceHandleRef(), 0,
            {{0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1,
              VK_SHADER_STAGE_COMPUTE_BIT, NULL}}));

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
