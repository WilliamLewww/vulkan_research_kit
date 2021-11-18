#include "vrk/buffer.h"
#include "vrk/command_buffer_group.h"
#include "vrk/command_pool.h"
#include "vrk/compute_pipeline_group.h"
#include "vrk/descriptor_pool.h"
#include "vrk/descriptor_set_group.h"
#include "vrk/descriptor_set_layout.h"
#include "vrk/device.h"
#include "vrk/fence.h"
#include "vrk/framebuffer.h"
#include "vrk/image.h"
#include "vrk/image_view.h"
#include "vrk/instance.h"
#include "vrk/pipeline_layout.h"
#include "vrk/render_pass.h"
#include "vrk/resource.h"
#include "vrk/shader_module.h"

#include <cstring>
#include <fstream>

int main(void) {
  std::vector<VkValidationFeatureEnableEXT> validationFeatureEnableList = {
      VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
      VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
      VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT};

  std::vector<VkValidationFeatureDisableEXT> validationFeatureDisableList = {};

  VkDebugUtilsMessageSeverityFlagBitsEXT debugUtilsMessageSeverityFlagBits =
      (VkDebugUtilsMessageSeverityFlagBitsEXT)(
          // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);

  VkDebugUtilsMessageTypeFlagBitsEXT debugUtilsMessageTypeFlagBits =
      (VkDebugUtilsMessageTypeFlagBitsEXT)(
          // VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);

  Instance *instance = new Instance(
      validationFeatureEnableList, validationFeatureDisableList,
      debugUtilsMessageSeverityFlagBits, debugUtilsMessageTypeFlagBits,
      "Demo Application", VK_MAKE_VERSION(1, 0, 0),
      {"VK_LAYER_KHRONOS_validation"}, {VK_EXT_DEBUG_UTILS_EXTENSION_NAME});

  std::cout << "Vulkan API " << instance->getVulkanVersionAPI().c_str()
            << std::endl;

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandleRef());

  VkPhysicalDevice activePhysicalDeviceHandle = deviceHandleList[0];
  for (VkPhysicalDevice deviceHandle : deviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties =
        Device::getPhysicalDeviceProperties(deviceHandle);

    if (physicalDeviceProperties.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {

      activePhysicalDeviceHandle = deviceHandle;
      break;
    }
  }

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList =
      Device::getQueueFamilyPropertiesList(activePhysicalDeviceHandle);

  uint32_t queueFamilyIndex = -1;
  for (uint32_t x = 0; x < queueFamilyPropertiesList.size(); x++) {
    if (queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndex = x;
      break;
    }
  }

  Device *device = new Device(activePhysicalDeviceHandle,
                              {{0, queueFamilyIndex, 1, {1.0f}}}, {}, {}, NULL);

  CommandPool *commandPool =
      new CommandPool(device->getDeviceHandleRef(), 0, queueFamilyIndex);

  CommandBufferGroup *commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandleRef(), commandPool->getCommandPoolHandleRef(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2);

  std::ifstream computeFile(
      Resource::findResource("resources/shaders/shader.comp.spv"),
      std::ios::binary | std::ios::ate);
  std::streamsize computeFileSize = computeFile.tellg();
  computeFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> computeShaderSource(computeFileSize / sizeof(uint32_t));
  computeFile.read((char *)computeShaderSource.data(), computeFileSize);
  computeFile.close();

  ShaderModule *computeShaderModule =
      new ShaderModule(device->getDeviceHandleRef(), computeShaderSource);

  ComputePipelineGroup::PipelineShaderStageCreateInfoParam computeStage = {
      .pipelineShaderStageCreateFlags = 0,
      .shaderStageFlagBits = VK_SHADER_STAGE_COMPUTE_BIT,
      .shaderModuleHandleRef = computeShaderModule->getShaderModuleHandleRef(),
      .entryPointName = "main",
      .specializationInfoPtr = NULL};

  DescriptorPool *descriptorPool =
      new DescriptorPool(device->getDeviceHandleRef(),
                         VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1,
                         {{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}});

  DescriptorSetLayout *descriptorSetLayout =
      new DescriptorSetLayout(device->getDeviceHandleRef(), 0,
                              {{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
                                VK_SHADER_STAGE_COMPUTE_BIT, NULL}});

  DescriptorSetGroup *descriptorSetGroup = new DescriptorSetGroup(
      device->getDeviceHandleRef(),
      descriptorPool->getDescriptorPoolHandleRef(),
      {descriptorSetLayout->getDescriptorSetLayoutHandleRef()});

  Buffer *dataBuffer =
      new Buffer(device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
                 sizeof(float) * 128, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                 VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  auto descriptorBufferInfo =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo{

          .buffer = dataBuffer->getBufferHandleRef(),
          .offset = 0,
          .range = VK_WHOLE_SIZE});

  descriptorSetGroup->updateDescriptorSets(
      {{0, 0, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, NULL,
        descriptorBufferInfo, NULL}},
      {});

  PipelineLayout *pipelineLayout = new PipelineLayout(
      device->getDeviceHandleRef(),
      {descriptorSetLayout->getDescriptorSetLayoutHandleRef()}, {});

  ComputePipelineGroup *computePipelineGroup = new ComputePipelineGroup(
      device->getDeviceHandleRef(),
      {{0, computeStage, pipelineLayout->getPipelineLayoutHandleRef(),
        VK_NULL_HANDLE, 0}});

  commandBufferGroup->beginRecording(
      0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  computePipelineGroup->bindPipelineCmd(
      0, commandBufferGroup->getCommandBufferHandleRef(0));

  descriptorSetGroup->bindDescriptorSetsCmd(
      commandBufferGroup->getCommandBufferHandleRef(0),
      VK_PIPELINE_BIND_POINT_COMPUTE,
      pipelineLayout->getPipelineLayoutHandleRef(), 0, {0}, {});

  computePipelineGroup->dispatchCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), 128, 1, 1);

  commandBufferGroup->endRecording(0);

  Fence *fence =
      new Fence(device->getDeviceHandleRef(), (VkFenceCreateFlagBits)0);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
                             {{{}, {}, {0}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  void *hostDataBuffer;
  dataBuffer->mapMemory(&hostDataBuffer, 0, 128 * sizeof(float));
  for (uint32_t x = 0; x < 128; x++) {
    printf("%f\n", ((float *)(hostDataBuffer))[x]);
  }
  dataBuffer->unmapMemory();

  delete fence;
  delete computePipelineGroup;
  delete pipelineLayout;
  delete dataBuffer;
  delete descriptorSetGroup;
  delete descriptorSetLayout;
  delete descriptorPool;
  delete computeShaderModule;
  delete commandBufferGroup;
  delete commandPool;
  delete device;
  delete instance;

  return 0;
}
