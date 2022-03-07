#include "vrk/buffer.h"
#include "vrk/command_buffer_group.h"
#include "vrk/command_pool.h"
#include "vrk/descriptor_pool.h"
#include "vrk/descriptor_set_group.h"
#include "vrk/descriptor_set_layout.h"
#include "vrk/device.h"
#include "vrk/fence.h"
#include "vrk/framebuffer.h"
#include "vrk/graphics_pipeline_group.h"
#include "vrk/image.h"
#include "vrk/image_view.h"
#include "vrk/instance.h"
#include "vrk/pipeline_layout.h"
#include "vrk/render_pass.h"
#include "vrk/resource.h"
#include "vrk/semaphore.h"
#include "vrk/shader_module.h"

#include "vrk/ray_tracing/acceleration_structure.h"
#include "vrk/ray_tracing/ray_tracing.h"
#include "vrk/ray_tracing/ray_tracing_pipeline_group.h"

#include <cstring>

void saveImage(std::string filename, void *buffer, uint32_t width,
               uint32_t height) {

  std::ofstream image(filename.c_str());

  image << "P3" << std::endl;
  image << width << " " << height << std::endl;
  image << "255" << std::endl;

  for (uint32_t y = 0; y < height; y++) {
    for (uint32_t x = 0; x < width; x++) {
      float r = ((float *)buffer)[((y * width + x) * 4) + 0];
      float g = ((float *)buffer)[((y * width + x) * 4) + 1];
      float b = ((float *)buffer)[((y * width + x) * 4) + 2];

      image << r * 255.0f << " " << g * 255.0f << " " << b * 255.0f << " ";
    }
    image << std::endl;
  }

  image.close();
}

int main(void) {
  std::vector<VkValidationFeatureEnableEXT> validationFeatureEnableList = {
      // VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
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

  std::vector<std::string> instanceExtensionList = {
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
  instanceExtensionList.insert(
      instanceExtensionList.end(),
      RAY_TRACING_PIPELINE_REQUIRED_INSTANCE_EXTENSION_LIST.begin(),
      RAY_TRACING_PIPELINE_REQUIRED_INSTANCE_EXTENSION_LIST.end());
  instanceExtensionList.erase(
      unique(instanceExtensionList.begin(), instanceExtensionList.end()),
      instanceExtensionList.end());

  Instance *instance = new Instance(
      validationFeatureEnableList, validationFeatureDisableList,
      debugUtilsMessageSeverityFlagBits, debugUtilsMessageTypeFlagBits,
      "Demo Application", VK_MAKE_VERSION(1, 0, 0),
      {"VK_LAYER_KHRONOS_validation"}, instanceExtensionList);

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

  VkPhysicalDeviceBufferDeviceAddressFeatures
      physicalDeviceBufferDeviceAddressFeatures = {
          .sType =
              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
          .pNext = NULL,
          .bufferDeviceAddress = VK_TRUE,
          .bufferDeviceAddressCaptureReplay = VK_FALSE,
          .bufferDeviceAddressMultiDevice = VK_FALSE};

  VkPhysicalDeviceAccelerationStructureFeaturesKHR
      physicalDeviceAccelerationStructureFeatures = {
          .sType =
              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
          .pNext = NULL,
          .accelerationStructure = VK_TRUE,
          .accelerationStructureCaptureReplay = VK_FALSE,
          .accelerationStructureIndirectBuild = VK_FALSE,
          .accelerationStructureHostCommands = VK_FALSE,
          .descriptorBindingAccelerationStructureUpdateAfterBind = VK_FALSE};

  VkPhysicalDeviceRayTracingPipelineFeaturesKHR
      physicalDeviceRayTracingPipelineFeatures = {
          .sType =
              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
          .pNext = NULL,
          .rayTracingPipeline = VK_TRUE,
          .rayTracingPipelineShaderGroupHandleCaptureReplay = VK_FALSE,
          .rayTracingPipelineShaderGroupHandleCaptureReplayMixed = VK_FALSE,
          .rayTracingPipelineTraceRaysIndirect = VK_FALSE,
          .rayTraversalPrimitiveCulling = VK_FALSE};

  std::vector<std::string> deviceExtensionList;
  deviceExtensionList.insert(
      deviceExtensionList.end(),
      RAY_TRACING_PIPELINE_REQUIRED_DEVICE_EXTENSION_LIST.begin(),
      RAY_TRACING_PIPELINE_REQUIRED_DEVICE_EXTENSION_LIST.end());
  deviceExtensionList.erase(
      unique(deviceExtensionList.begin(), deviceExtensionList.end()),
      deviceExtensionList.end());

  Device *device =
      new Device(activePhysicalDeviceHandle, {{0, queueFamilyIndex, 1, {1.0f}}},
                 {}, deviceExtensionList, NULL,
                 {&physicalDeviceBufferDeviceAddressFeatures,
                  &physicalDeviceAccelerationStructureFeatures,
                  &physicalDeviceRayTracingPipelineFeatures});

  CommandPool *commandPool = new CommandPool(
      device->getDeviceHandleRef(),
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueFamilyIndex);

  CommandBufferGroup *commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandleRef(), commandPool->getCommandPoolHandleRef(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2);

  float vertices[12] = {-0.5f, -0.5f, 0.5f, 0.5f,  -0.5f, 0.5f,
                        0.5f,  0.5f,  0.5f, -0.5f, 0.5f,  0.5f};

  uint32_t indices[6] = {0, 1, 2, 2, 3, 0};

  VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
      .pNext = NULL,
      .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
      .deviceMask = 0};

  Buffer *vertexBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      sizeof(float) * 12,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {&memoryAllocateFlagsInfo});

  void *hostVertexBuffer;
  vertexBuffer->mapMemory(&hostVertexBuffer, 0, 12 * sizeof(float));
  memcpy(hostVertexBuffer, vertices, 12 * sizeof(float));
  vertexBuffer->unmapMemory();

  Buffer *indexBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      sizeof(uint32_t) * 6,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {&memoryAllocateFlagsInfo});

  void *hostIndexBuffer;
  indexBuffer->mapMemory(&hostIndexBuffer, 0, 6 * sizeof(uint32_t));
  memcpy(hostIndexBuffer, indices, 6 * sizeof(uint32_t));
  indexBuffer->unmapMemory();

  AccelerationStructure::AccelerationStructureGeometryDataParam
      accelerationStructureGeometryDataParam = {

          .accelerationStructureGeometryTrianglesDataParam = {
              .vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
              .vertexData = {.deviceAddress =
                                 vertexBuffer->getBufferDeviceAddress()},
              .vertexStride = sizeof(float) * 3,
              .maxVertex = 4,
              .indexType = VK_INDEX_TYPE_UINT32,
              .indexData = {.deviceAddress =
                                indexBuffer->getBufferDeviceAddress()},
              .transformData = {.deviceAddress = 0}}};

  AccelerationStructure::AccelerationStructureBuildSizesInfoParam
      accelerationStructureBuildSizesInfoParam =
          AccelerationStructure::getAccelerationStructureBuildSizes(
              device->getDeviceHandleRef(),
              VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
              VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, 0,
              {{VK_GEOMETRY_TYPE_TRIANGLES_KHR,
                accelerationStructureGeometryDataParam, 0}},
              {1});

  Buffer *accelerationStructureBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      accelerationStructureBuildSizesInfoParam.accelerationStructureSize,
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {});

  AccelerationStructure *accelerationStructure = new AccelerationStructure(
      device->getDeviceHandleRef(), 0,
      accelerationStructureBuffer->getBufferHandleRef(), 0,
      accelerationStructureBuildSizesInfoParam.accelerationStructureSize,
      VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, 0);

  commandBufferGroup->beginRecording(
      1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  Buffer *accelerationStructureScratchBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      accelerationStructureBuildSizesInfoParam.buildScratchSize,
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {&memoryAllocateFlagsInfo});

  AccelerationStructure::AccelerationStructureBuildGeometryInfoParam
      accelerationStructureBuildGeometryInfoParam = {

          .accelerationStructureType =
              VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
          .buildAccelerationStructureFlags = 0,
          .buildAccelerationStructureMode =
              VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
          .srcAccelerationStructureHandle = VK_NULL_HANDLE,
          .dstAccelerationStructureHandleRef =
              accelerationStructure->getAccelerationStructureHandleRef(),
          .accelerationStructureGeometryParamList =
              {{VK_GEOMETRY_TYPE_TRIANGLES_KHR,
                accelerationStructureGeometryDataParam,
                VK_GEOMETRY_OPAQUE_BIT_KHR}},
          .scratchDataDeviceOrHostAddress = {
              .deviceAddress = accelerationStructureScratchBuffer
                                   ->getBufferDeviceAddress()}};

  VkAccelerationStructureBuildRangeInfoKHR
      accelerationStructureBuildRangeInfoKHR = {

          .primitiveCount = 2,
          .primitiveOffset = 0,
          .firstVertex = 0,
          .transformOffset = 0};

  AccelerationStructure::buildAccelerationStructures(
      device->getDeviceHandleRef(),
      commandBufferGroup->getCommandBufferHandleRef(1),
      {accelerationStructureBuildGeometryInfoParam},
      {accelerationStructureBuildRangeInfoKHR});

  commandBufferGroup->endRecording(1);

  Fence *fence =
      new Fence(device->getDeviceHandleRef(), (VkFenceCreateFlagBits)0);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
                             {{{}, {}, {1}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  commandBufferGroup->reset(1, 0);

  VkAccelerationStructureInstanceKHR accelerationStructureInstance = {
      .transform = {.matrix = {{1.0, 0.0, 0.0, 0.0},
                               {0.0, 1.0, 0.0, 0.0},
                               {0.0, 0.0, 1.0, 0.0}}},
      .instanceCustomIndex = 0,
      .mask = 0xFF,
      .instanceShaderBindingTableRecordOffset = 0,
      .flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR,
      .accelerationStructureReference =
          accelerationStructure->getAccelerationStructureDeviceAddress()};

  Buffer *geometryInstanceBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      sizeof(VkAccelerationStructureInstanceKHR),
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {&memoryAllocateFlagsInfo});

  void *hostGeometryInstanceBuffer;
  geometryInstanceBuffer->mapMemory(&hostGeometryInstanceBuffer, 0,
                                    sizeof(VkAccelerationStructureInstanceKHR));
  memcpy(hostGeometryInstanceBuffer, &accelerationStructureInstance,
         sizeof(VkAccelerationStructureInstanceKHR));
  geometryInstanceBuffer->unmapMemory();

  AccelerationStructure::AccelerationStructureGeometryDataParam
      topLevelAccelerationStructureGeometryDataParam = {

          .accelerationStructureGeometryInstancesDataParam = {
              .arrayOfPointers = false,
              .data = {.deviceAddress =
                           geometryInstanceBuffer->getBufferDeviceAddress()},
          }};

  AccelerationStructure::AccelerationStructureBuildSizesInfoParam
      topLevelAccelerationStructureBuildSizesInfoParam =
          AccelerationStructure::getAccelerationStructureBuildSizes(
              device->getDeviceHandleRef(),
              VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
              VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, 0,
              {{VK_GEOMETRY_TYPE_INSTANCES_KHR,
                topLevelAccelerationStructureGeometryDataParam,
                VK_GEOMETRY_OPAQUE_BIT_KHR}},
              {1});

  Buffer *topLevelAccelerationStructureBuffer =
      new Buffer(device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
                 topLevelAccelerationStructureBuildSizesInfoParam
                     .accelerationStructureSize,
                 VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
                 VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {});

  AccelerationStructure *topLevelAccelerationStructure =
      new AccelerationStructure(
          device->getDeviceHandleRef(), 0,
          topLevelAccelerationStructureBuffer->getBufferHandleRef(), 0,
          topLevelAccelerationStructureBuildSizesInfoParam
              .accelerationStructureSize,
          VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR, 0);

  commandBufferGroup->beginRecording(
      1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  Buffer *topLevelAccelerationStructureScratchBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      topLevelAccelerationStructureBuildSizesInfoParam.buildScratchSize,
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {&memoryAllocateFlagsInfo});

  AccelerationStructure::AccelerationStructureBuildGeometryInfoParam
      topLevelAccelerationStructureBuildGeometryInfoParam = {

          .accelerationStructureType =
              VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
          .buildAccelerationStructureFlags = 0,
          .buildAccelerationStructureMode =
              VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
          .srcAccelerationStructureHandle = VK_NULL_HANDLE,
          .dstAccelerationStructureHandleRef =
              topLevelAccelerationStructure
                  ->getAccelerationStructureHandleRef(),
          .accelerationStructureGeometryParamList =
              {{VK_GEOMETRY_TYPE_INSTANCES_KHR,
                topLevelAccelerationStructureGeometryDataParam, 0}},
          .scratchDataDeviceOrHostAddress = {
              .deviceAddress = topLevelAccelerationStructureScratchBuffer
                                   ->getBufferDeviceAddress()}};

  VkAccelerationStructureBuildRangeInfoKHR
      topLevelAccelerationStructureBuildRangeInfoKHR = {

          .primitiveCount = 1,
          .primitiveOffset = 0,
          .firstVertex = 0,
          .transformOffset = 0};

  AccelerationStructure::buildAccelerationStructures(
      device->getDeviceHandleRef(),
      commandBufferGroup->getCommandBufferHandleRef(1),
      {topLevelAccelerationStructureBuildGeometryInfoParam},
      {topLevelAccelerationStructureBuildRangeInfoKHR});

  commandBufferGroup->endRecording(1);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
                             {{{}, {}, {1}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  commandBufferGroup->reset(1, 0);

  Image *colorImage =
      new Image(device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
                VK_IMAGE_TYPE_2D, VK_FORMAT_R32G32B32A32_SFLOAT, {800, 600, 1},
                1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
                VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  ImageView *colorImageView = new ImageView(
      device->getDeviceHandleRef(), colorImage->getImageHandleRef(), 0,
      VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32G32B32A32_SFLOAT,
      {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
       VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
      {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

  DescriptorPool *descriptorPool =
      new DescriptorPool(device->getDeviceHandleRef(),
                         VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1,
                         {{VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1},
                          {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}});

  DescriptorSetLayout *descriptorSetLayout = new DescriptorSetLayout(
      device->getDeviceHandleRef(), 0,
      {{0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1,
        VK_SHADER_STAGE_RAYGEN_BIT_KHR, NULL},
       {1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_RAYGEN_BIT_KHR,
        NULL}});

  DescriptorSetGroup *descriptorSetGroup = new DescriptorSetGroup(
      device->getDeviceHandleRef(),
      descriptorPool->getDescriptorPoolHandleRef(),
      {descriptorSetLayout->getDescriptorSetLayoutHandleRef()});

  VkWriteDescriptorSetAccelerationStructureKHR
      writeDescriptorSetAccelerationStructure = {

          .sType =
              VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR,
          .pNext = NULL,
          .accelerationStructureCount = 1,
          .pAccelerationStructures = &topLevelAccelerationStructure
                                          ->getAccelerationStructureHandleRef(),
      };

  VkDescriptorImageInfo descriptorImageInfo = {
      .sampler = VK_NULL_HANDLE,
      .imageView = colorImageView->getImageViewHandleRef(),
      .imageLayout = VK_IMAGE_LAYOUT_GENERAL};

  descriptorSetGroup->updateDescriptorSets(
      {{0,
        0,
        0,
        1,
        VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
        {},
        {},
        {},
        &writeDescriptorSetAccelerationStructure},
       {0,
        1,
        0,
        1,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        {descriptorImageInfo},
        {},
        {}}},
      {});

  PipelineLayout *pipelineLayout = new PipelineLayout(
      device->getDeviceHandleRef(),
      {descriptorSetLayout->getDescriptorSetLayoutHandleRef()}, {});

  std::ifstream rayGenerationFile(
      Resource::findResource("resources/shaders/shader.rgen.spv"),
      std::ios::binary | std::ios::ate);
  std::streamsize rayGenerationFileSize = rayGenerationFile.tellg();
  rayGenerationFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> rayGenerationShaderSource(rayGenerationFileSize /
                                                  sizeof(uint32_t));
  rayGenerationFile.read((char *)rayGenerationShaderSource.data(),
                         rayGenerationFileSize);
  rayGenerationFile.close();

  ShaderModule *rayGenerationShaderModule =
      new ShaderModule(device->getDeviceHandleRef(), rayGenerationShaderSource);

  std::ifstream rayMissFile(
      Resource::findResource("resources/shaders/shader.rmiss.spv"),
      std::ios::binary | std::ios::ate);
  std::streamsize rayMissFileSize = rayMissFile.tellg();
  rayMissFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> rayMissShaderSource(rayMissFileSize / sizeof(uint32_t));
  rayMissFile.read((char *)rayMissShaderSource.data(), rayMissFileSize);
  rayMissFile.close();

  ShaderModule *rayMissShaderModule =
      new ShaderModule(device->getDeviceHandleRef(), rayMissShaderSource);

  std::ifstream rayClosestHitFile(
      Resource::findResource("resources/shaders/shader.rchit.spv"),
      std::ios::binary | std::ios::ate);
  std::streamsize rayClosestHitFileSize = rayClosestHitFile.tellg();
  rayClosestHitFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> rayClosestHitShaderSource(rayClosestHitFileSize /
                                                  sizeof(uint32_t));
  rayClosestHitFile.read((char *)rayClosestHitShaderSource.data(),
                         rayClosestHitFileSize);
  rayClosestHitFile.close();

  ShaderModule *rayClosestHitShaderModule =
      new ShaderModule(device->getDeviceHandleRef(), rayClosestHitShaderSource);

  RayTracingPipelineGroup::PipelineShaderStageCreateInfoParam
      rayGenerationStage = {
          .pipelineShaderStageCreateFlags = 0,
          .shaderStageFlagBits = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
          .shaderModuleHandleRef =
              rayGenerationShaderModule->getShaderModuleHandleRef(),
          .entryPointName = "main",
          .specializationInfoPtr = NULL};

  RayTracingPipelineGroup::PipelineShaderStageCreateInfoParam rayMissStage = {
      .pipelineShaderStageCreateFlags = 0,
      .shaderStageFlagBits = VK_SHADER_STAGE_MISS_BIT_KHR,
      .shaderModuleHandleRef = rayMissShaderModule->getShaderModuleHandleRef(),
      .entryPointName = "main",
      .specializationInfoPtr = NULL};

  RayTracingPipelineGroup::PipelineShaderStageCreateInfoParam
      rayClosestHitStage = {
          .pipelineShaderStageCreateFlags = 0,
          .shaderStageFlagBits = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
          .shaderModuleHandleRef =
              rayClosestHitShaderModule->getShaderModuleHandleRef(),
          .entryPointName = "main",
          .specializationInfoPtr = NULL};

  RayTracingPipelineGroup *rayTracingPipelineGroup =
      new RayTracingPipelineGroup(
          device->getDeviceHandleRef(),
          {{0,
            {rayGenerationStage, rayMissStage, rayClosestHitStage},
            {{VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR, 0,
              VK_SHADER_UNUSED_KHR, VK_SHADER_UNUSED_KHR, VK_SHADER_UNUSED_KHR,
              NULL},
             {VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR, 1,
              VK_SHADER_UNUSED_KHR, VK_SHADER_UNUSED_KHR, VK_SHADER_UNUSED_KHR,
              NULL},
             {VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR,
              VK_SHADER_UNUSED_KHR, 2, VK_SHADER_UNUSED_KHR,
              VK_SHADER_UNUSED_KHR, NULL}},
            16,
            NULL,
            NULL,
            NULL,
            pipelineLayout->getPipelineLayoutHandleRef(),
            VK_NULL_HANDLE,
            0}});

  VkPhysicalDeviceRayTracingPipelinePropertiesKHR
      physicalDeviceRayTracingPipelineProperties = {
          .sType =
              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR,
          .pNext = NULL};

  device->getPhysicalDeviceProperties2(
      activePhysicalDeviceHandle,
      {&physicalDeviceRayTracingPipelineProperties});

  VkDeviceSize shaderBindingTableSize =
      physicalDeviceRayTracingPipelineProperties.shaderGroupHandleSize * 3;

  Buffer *shaderBindingTableBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      shaderBindingTableSize,
      VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {&memoryAllocateFlagsInfo});

  std::shared_ptr<char> shaderHandleBuffer(new char[shaderBindingTableSize]);
  rayTracingPipelineGroup->getRayTracingShaderGroupHandles(
      0, 0, 3, shaderBindingTableSize, shaderHandleBuffer);

  void *hostShaderBindingTableBuffer;
  shaderBindingTableBuffer->mapMemory(&hostShaderBindingTableBuffer, 0,
                                      shaderBindingTableSize);
  for (uint32_t x = 0; x < 3; x++) {
    memcpy(hostShaderBindingTableBuffer,
           shaderHandleBuffer.get() +
               x * physicalDeviceRayTracingPipelineProperties
                       .shaderGroupHandleSize,
           physicalDeviceRayTracingPipelineProperties.shaderGroupHandleSize);
    hostShaderBindingTableBuffer =
        (char *)hostShaderBindingTableBuffer +
        physicalDeviceRayTracingPipelineProperties.shaderGroupBaseAlignment;
  }
  shaderBindingTableBuffer->unmapMemory();

  VkDeviceSize progSize =
      physicalDeviceRayTracingPipelineProperties.shaderGroupBaseAlignment;
  VkDeviceSize sbtSize = progSize * (VkDeviceSize)4;
  VkDeviceSize rayGenOffset = 0u * progSize;
  VkDeviceSize missOffset = 1u * progSize;
  VkDeviceSize hitGroupOffset = 2u * progSize;

  const VkStridedDeviceAddressRegionKHR rgenShaderBindingTable = {
      .deviceAddress =
          shaderBindingTableBuffer->getBufferDeviceAddress() + 0u * progSize,
      .stride = sbtSize,
      .size = sbtSize * 1};

  const VkStridedDeviceAddressRegionKHR rmissShaderBindingTable = {
      .deviceAddress =
          shaderBindingTableBuffer->getBufferDeviceAddress() + 1u * progSize,
      .stride = sbtSize,
      .size = sbtSize * 1};

  const VkStridedDeviceAddressRegionKHR rchitShaderBindingTable = {
      .deviceAddress =
          shaderBindingTableBuffer->getBufferDeviceAddress() + 2u * progSize,
      .stride = sbtSize,
      .size = sbtSize * 1};

  const VkStridedDeviceAddressRegionKHR callableShaderBindingTable = {};

  commandBufferGroup->beginRecording(
      0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  rayTracingPipelineGroup->bindPipelineCmd(
      0, commandBufferGroup->getCommandBufferHandleRef(0));

  descriptorSetGroup->bindDescriptorSetsCmd(
      commandBufferGroup->getCommandBufferHandleRef(0),
      VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
      pipelineLayout->getPipelineLayoutHandleRef(), 0, {0}, {});

  commandBufferGroup->createPipelineBarrierCmd(
      0, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      0, {}, {},
      {{VK_ACCESS_MEMORY_WRITE_BIT,
        VK_ACCESS_MEMORY_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL,
        queueFamilyIndex,
        queueFamilyIndex,
        colorImage->getImageHandleRef(),
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

  rayTracingPipelineGroup->traceRaysCmd(
      commandBufferGroup->getCommandBufferHandleRef(0),
      std::make_shared<VkStridedDeviceAddressRegionKHR>(rgenShaderBindingTable),
      std::make_shared<VkStridedDeviceAddressRegionKHR>(
          rmissShaderBindingTable),
      std::make_shared<VkStridedDeviceAddressRegionKHR>(
          rchitShaderBindingTable),
      std::make_shared<VkStridedDeviceAddressRegionKHR>(
          callableShaderBindingTable),
      800, 600, 1);

  commandBufferGroup->createPipelineBarrierCmd(
      0, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, {}, {},
      {{VK_ACCESS_MEMORY_WRITE_BIT,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        queueFamilyIndex,
        queueFamilyIndex,
        colorImage->getImageHandleRef(),
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

  commandBufferGroup->endRecording(0);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
                             {{{}, {}, {0}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  Buffer *imageBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      800 * 600 * 4 * sizeof(float),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  commandBufferGroup->beginRecording(
      1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  imageBuffer->copyFromImageCmd(
      commandBufferGroup->getCommandBufferHandleRef(1),
      colorImage->getImageHandleRef(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      {{0,
        0,
        0,
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        {0, 0, 0},
        {800, 600, 1}}});

  commandBufferGroup->endRecording(1);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
                             {{{}, {}, {1}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  void *hostImageBuffer;
  imageBuffer->mapMemory(&hostImageBuffer, 0, 800 * 600 * 4 * sizeof(float));
  saveImage("image.ppm", hostImageBuffer, 800, 600);
  imageBuffer->unmapMemory();

  delete imageBuffer;
  delete shaderBindingTableBuffer;
  delete rayTracingPipelineGroup;
  delete rayClosestHitShaderModule;
  delete rayMissShaderModule;
  delete rayGenerationShaderModule;
  delete pipelineLayout;
  delete descriptorSetLayout;
  delete descriptorPool;
  delete colorImageView;
  delete colorImage;
  delete topLevelAccelerationStructureScratchBuffer;
  delete topLevelAccelerationStructure;
  delete topLevelAccelerationStructureBuffer;
  delete geometryInstanceBuffer;
  delete fence;
  delete accelerationStructureScratchBuffer;
  delete accelerationStructure;
  delete accelerationStructureBuffer;
  delete indexBuffer;
  delete vertexBuffer;
  delete commandBufferGroup;
  delete commandPool;
  delete device;
  delete instance;

  return 0;
}
