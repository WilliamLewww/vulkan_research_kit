#include "vrk/instance.h"
#include "vrk/device.h"
#include "vrk/buffer.h"
#include "vrk/command_pool.h"
#include "vrk/command_buffer_group.h"
#include "vrk/fence.h"
#include "vrk/render_pass.h"
#include "vrk/image.h"
#include "vrk/image_view.h"
#include "vrk/framebuffer.h"
#include "vrk/shader_module.h"
#include "vrk/resource.h"
#include "vrk/graphics_pipeline_group.h"
#include "vrk/descriptor_pool.h"
#include "vrk/descriptor_set_layout.h"
#include "vrk/descriptor_set_group.h"
#include "vrk/pipeline_layout.h"
#include "vrk/buffer.h"
#include "vrk/fence.h"
#include "vrk/semaphore.h"

#include "vrk/ray_tracing/acceleration_structure.h"

#include <cstring>

void saveImage(std::string filename, void* buffer, uint32_t width,
    uint32_t height) {

  std::ofstream image(filename.c_str());

  image << "P3" << std::endl;
  image << width << " " << height << std::endl;
  image << "255" << std::endl;

  for (uint32_t y = 0; y < height; y++) {
    for (uint32_t x = 0; x < width; x++) {
      float r = ((float*)buffer)[((y * width + x) * 4) + 0];
      float g = ((float*)buffer)[((y * width + x) * 4) + 1];
      float b = ((float*)buffer)[((y * width + x) * 4) + 2];

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
    VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
  };

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

  Instance* instance = new Instance(
      validationFeatureEnableList,
      validationFeatureDisableList,
      debugUtilsMessageSeverityFlagBits,
      debugUtilsMessageTypeFlagBits,
      "Demo Application",
      VK_MAKE_VERSION(1, 0, 0),
      {"VK_LAYER_KHRONOS_validation"},
      {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, 
          VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});

  std::cout << "Vulkan API " << instance->getVulkanVersionAPI().c_str()
      << std::endl;

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandleRef());

  VkPhysicalDevice activePhysicalDeviceHandle;
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

    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
    .pNext = NULL,
    .bufferDeviceAddress = VK_TRUE,
    .bufferDeviceAddressCaptureReplay = VK_FALSE,
    .bufferDeviceAddressMultiDevice = VK_FALSE
  };

  VkPhysicalDeviceAccelerationStructureFeaturesKHR
      physicalDeviceAccelerationStructureFeatures = {

    .sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
    .pNext = NULL,
    .accelerationStructure = VK_TRUE,
    .accelerationStructureCaptureReplay = VK_FALSE,
    .accelerationStructureIndirectBuild = VK_FALSE,
    .accelerationStructureHostCommands = VK_FALSE,
    .descriptorBindingAccelerationStructureUpdateAfterBind = VK_FALSE
  };

  VkPhysicalDeviceRayQueryFeaturesKHR
      physicalDeviceRayQueryFeatures = {

    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
    .pNext = NULL,
    .rayQuery = VK_TRUE
  };

  Device* device = new Device(activePhysicalDeviceHandle,
      {{0, queueFamilyIndex, 1, {1.0f}}},
      {},
      {"VK_KHR_ray_query", "VK_KHR_spirv_1_4", "VK_KHR_shader_float_controls", 
          "VK_KHR_acceleration_structure", "VK_EXT_descriptor_indexing", 
          "VK_KHR_maintenance3", "VK_KHR_buffer_device_address", 
          "VK_KHR_deferred_host_operations"},
      NULL,
      {&physicalDeviceBufferDeviceAddressFeatures,
          &physicalDeviceAccelerationStructureFeatures,
          &physicalDeviceRayQueryFeatures});

  CommandPool* commandPool = new CommandPool(device->getDeviceHandleRef(),
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      queueFamilyIndex);

  CommandBufferGroup* commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandleRef(), commandPool->getCommandPoolHandleRef(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2);

  float vertices[12] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
  };

  uint32_t indices[6] = {
    0, 1, 2, 2, 3, 0
  };

  VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
    .pNext = NULL,
    .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
    .deviceMask = 0
  };

  Buffer* vertexBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      sizeof(float) * 12,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      {&memoryAllocateFlagsInfo});

  void* hostVertexBuffer;
  vertexBuffer->mapMemory(&hostVertexBuffer, 0, 12 * sizeof(float));
  memcpy(hostVertexBuffer, vertices, 12 * sizeof(float));
  vertexBuffer->unmapMemory();

  Buffer* indexBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      sizeof(uint32_t) * 6,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      {&memoryAllocateFlagsInfo});

  void* hostIndexBuffer;
  indexBuffer->mapMemory(&hostIndexBuffer, 0, 6 * sizeof(uint32_t));
  memcpy(hostIndexBuffer, indices, 6 * sizeof(uint32_t));
  indexBuffer->unmapMemory();

  AccelerationStructure::AccelerationStructureGeometryDataParam
      accelerationStructureGeometryDataParam = {

    .accelerationStructureGeometryTrianglesDataParam = {
      .vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
      .vertexData = {
        .deviceAddress = vertexBuffer->getBufferDeviceAddress()
      },
      .vertexStride = sizeof(float) * 3,
      .maxVertex = 3,
      .indexType = VK_INDEX_TYPE_UINT32,
      .indexData = {
        .deviceAddress = indexBuffer->getBufferDeviceAddress()
      },
      .transformData = {
        .deviceAddress = 0
      }
    }
  };

  AccelerationStructure::AccelerationStructureBuildSizesInfoParam
      accelerationStructureBuildSizesInfoParam =
      AccelerationStructure::getAccelerationStructureBuildSizes(
      device->getDeviceHandleRef(),
      VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
      VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
      0,
      {{VK_GEOMETRY_TYPE_TRIANGLES_KHR,
          accelerationStructureGeometryDataParam,
          0}},
      {1});

  Buffer* accelerationStructureBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      accelerationStructureBuildSizesInfoParam.accelerationStructureSize,
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      {});

  AccelerationStructure* accelerationStructure = new AccelerationStructure(
      device->getDeviceHandleRef(),
      0,
      accelerationStructureBuffer->getBufferHandleRef(),
      0,
      accelerationStructureBuildSizesInfoParam.accelerationStructureSize,
      VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
      0);

  commandBufferGroup->beginRecording(1,
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  Buffer* accelerationStructureScratchBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      accelerationStructureBuildSizesInfoParam.buildScratchSize,
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      {&memoryAllocateFlagsInfo});

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
            0}},
    .scratchDataDeviceOrHostAddress = {
      .deviceAddress = accelerationStructureScratchBuffer->
          getBufferDeviceAddress()
    }
  };

  VkAccelerationStructureBuildRangeInfoKHR
      accelerationStructureBuildRangeInfoKHR = {

    .primitiveCount = 1,
    .primitiveOffset = 0,
    .firstVertex = 0,
    .transformOffset = 0
  };

  AccelerationStructure::buildAccelerationStructures(
      device->getDeviceHandleRef(),
      commandBufferGroup->getCommandBufferHandleRef(1),
      {accelerationStructureBuildGeometryInfoParam},
      {accelerationStructureBuildRangeInfoKHR});

  commandBufferGroup->endRecording(1);

  Fence* fence = new Fence(device->getDeviceHandleRef(),
      (VkFenceCreateFlagBits)0);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
      {{{}, {}, {1}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  commandBufferGroup->reset(1, 0);

  VkAccelerationStructureInstanceKHR accelerationStructureInstance = {
    .transform = {
      .matrix = {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0}
      }
    },
    .instanceCustomIndex = 0,
    .mask = 0xFF,
    .instanceShaderBindingTableRecordOffset = 0,
    .flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR,
    .accelerationStructureReference = accelerationStructure->getAccelerationStructureDeviceAddress()
  };

  Buffer* geometryInstanceBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      sizeof(VkAccelerationStructureInstanceKHR),
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      {&memoryAllocateFlagsInfo});

  void* hostGeometryInstanceBuffer;
  geometryInstanceBuffer->mapMemory(&hostGeometryInstanceBuffer, 0, sizeof(VkAccelerationStructureInstanceKHR));
  memcpy(hostGeometryInstanceBuffer, &accelerationStructureInstance, sizeof(VkAccelerationStructureInstanceKHR));
  geometryInstanceBuffer->unmapMemory();

  AccelerationStructure::AccelerationStructureGeometryDataParam
      topLevelAccelerationStructureGeometryDataParam = {

    .accelerationStructureGeometryInstancesDataParam = {
      .arrayOfPointers = false,
      .data = {
        .deviceAddress = geometryInstanceBuffer->getBufferDeviceAddress()
      },
    }
  };

  AccelerationStructure::AccelerationStructureBuildSizesInfoParam
      topLevelAccelerationStructureBuildSizesInfoParam =
      AccelerationStructure::getAccelerationStructureBuildSizes(
      device->getDeviceHandleRef(),
      VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
      VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
      0,
      {{VK_GEOMETRY_TYPE_INSTANCES_KHR,
          topLevelAccelerationStructureGeometryDataParam,
          0}},
      {1});

  Buffer* topLevelAccelerationStructureBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      topLevelAccelerationStructureBuildSizesInfoParam.accelerationStructureSize,
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      {});

  AccelerationStructure* topLevelAccelerationStructure = new AccelerationStructure(
      device->getDeviceHandleRef(),
      0,
      topLevelAccelerationStructureBuffer->getBufferHandleRef(),
      0,
      topLevelAccelerationStructureBuildSizesInfoParam.accelerationStructureSize,
      VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
      0);

  commandBufferGroup->beginRecording(1,
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  Buffer* topLevelAccelerationStructureScratchBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      topLevelAccelerationStructureBuildSizesInfoParam.buildScratchSize,
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      {&memoryAllocateFlagsInfo});

  AccelerationStructure::AccelerationStructureBuildGeometryInfoParam
      topLevelAccelerationStructureBuildGeometryInfoParam = {

    .accelerationStructureType =
        VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
    .buildAccelerationStructureFlags = 0,
    .buildAccelerationStructureMode =
        VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR,
    .srcAccelerationStructureHandle = VK_NULL_HANDLE,
    .dstAccelerationStructureHandleRef =
        topLevelAccelerationStructure->getAccelerationStructureHandleRef(),
    .accelerationStructureGeometryParamList =
        {{VK_GEOMETRY_TYPE_INSTANCES_KHR,
            topLevelAccelerationStructureGeometryDataParam,
            0}},
    .scratchDataDeviceOrHostAddress = {
      .deviceAddress = topLevelAccelerationStructureScratchBuffer->
          getBufferDeviceAddress()
    }
  };

  VkAccelerationStructureBuildRangeInfoKHR
      topLevelAccelerationStructureBuildRangeInfoKHR = {

    .primitiveCount = 1,
    .primitiveOffset = 0,
    .firstVertex = 0,
    .transformOffset = 0
  };

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

  std::vector<VkAttachmentReference> attachmentReferenceList = {
    {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
  };

  RenderPass* renderPass = new RenderPass(device->getDeviceHandleRef(),
      (VkRenderPassCreateFlagBits)0,
      {{0,
          VK_FORMAT_R32G32B32A32_SFLOAT,
          VK_SAMPLE_COUNT_1_BIT,
          VK_ATTACHMENT_LOAD_OP_CLEAR,
          VK_ATTACHMENT_STORE_OP_STORE,
          VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          VK_ATTACHMENT_STORE_OP_DONT_CARE,
          VK_IMAGE_LAYOUT_UNDEFINED,
          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}},
      {{0,
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          0,
          NULL,
          1,
          attachmentReferenceList.data(),
          NULL,
          NULL,
          0,
          NULL,
      }},
      {});

  Image* colorImage = new Image(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      VK_IMAGE_TYPE_2D,
      VK_FORMAT_R32G32B32A32_SFLOAT,
      {800, 600, 1},
      1,
      1,
      VK_SAMPLE_COUNT_1_BIT,
      VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  ImageView* colorImageView = new ImageView(device->getDeviceHandleRef(),
      colorImage->getImageHandleRef(),
      0,
      VK_IMAGE_VIEW_TYPE_2D,
      VK_FORMAT_R32G32B32A32_SFLOAT,
      {VK_COMPONENT_SWIZZLE_IDENTITY,
          VK_COMPONENT_SWIZZLE_IDENTITY,
          VK_COMPONENT_SWIZZLE_IDENTITY,
          VK_COMPONENT_SWIZZLE_IDENTITY},
      {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

  Framebuffer* framebuffer = new Framebuffer(device->getDeviceHandleRef(),
      renderPass->getRenderPassHandleRef(),
      {colorImageView->getImageViewHandleRef()},
      (VkFramebufferCreateFlags)0,
      800,
      600,
      1);

  std::ifstream vertexFile(
      Resource::findResource("resources/shaders/shader.vert.spv"),
      std::ios::binary | std::ios::ate);
  std::streamsize vertexFileSize = vertexFile.tellg();
  vertexFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> vertexShaderSource(vertexFileSize / sizeof(uint32_t));
  vertexFile.read((char*)vertexShaderSource.data(), vertexFileSize);
  vertexFile.close();

  ShaderModule* vertexShaderModule = new ShaderModule(
      device->getDeviceHandleRef(), vertexShaderSource);

  std::ifstream fragmentFile(
      Resource::findResource("resources/shaders/shader.frag.spv"),
      std::ios::binary | std::ios::ate);
  std::streamsize fragmentFileSize = fragmentFile.tellg();
  fragmentFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> fragmentShaderSource(
      fragmentFileSize / sizeof(uint32_t));
  fragmentFile.read((char*)fragmentShaderSource.data(), fragmentFileSize);
  fragmentFile.close();

  ShaderModule* fragmentShaderModule = new ShaderModule(
      device->getDeviceHandleRef(), fragmentShaderSource);

  GraphicsPipelineGroup::PipelineShaderStageCreateInfoParam vertexStage = {
    .pipelineShaderStageCreateFlags = 0,
    .shaderStageFlagBits = VK_SHADER_STAGE_VERTEX_BIT,
    .shaderModuleHandleRef = vertexShaderModule->getShaderModuleHandleRef(),
    .entryPointName = "main",
    .specializationInfoPtr = NULL
  };

  GraphicsPipelineGroup::PipelineShaderStageCreateInfoParam fragmentStage = {
    .pipelineShaderStageCreateFlags = 0,
    .shaderStageFlagBits = VK_SHADER_STAGE_FRAGMENT_BIT,
    .shaderModuleHandleRef = fragmentShaderModule->getShaderModuleHandleRef(),
    .entryPointName = "main",
    .specializationInfoPtr = NULL
  };

  DescriptorPool* descriptorPool = new DescriptorPool(
      device->getDeviceHandleRef(),
      VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
      1,
      {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
       {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1}});

  DescriptorSetLayout* descriptorSetLayout = new DescriptorSetLayout(
      device->getDeviceHandleRef(),
      0,
      {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
          NULL},
       {1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1,
          VK_SHADER_STAGE_FRAGMENT_BIT, NULL}});

  DescriptorSetGroup* descriptorSetGroup = new DescriptorSetGroup(
      device->getDeviceHandleRef(),
      descriptorPool->getDescriptorPoolHandleRef(),
      {descriptorSetLayout->getDescriptorSetLayoutHandleRef()});

  float colors[3] = {
    1.0f, 0.7f, 0.5f,
  };

  Buffer* colorBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      sizeof(float) * 3,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  void* hostColorBuffer;
  colorBuffer->mapMemory(&hostColorBuffer, 0, 3 * sizeof(float));
  memcpy(hostColorBuffer, colors, 3 * sizeof(float));
  colorBuffer->unmapMemory();

  auto descriptorBufferInfo =
      std::make_shared<VkDescriptorBufferInfo>(VkDescriptorBufferInfo {

    .buffer = colorBuffer->getBufferHandleRef(),
    .offset = 0,
    .range = VK_WHOLE_SIZE
  });

  VkWriteDescriptorSetAccelerationStructureKHR
      writeDescriptorSetAccelerationStructure = {

    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR,
    .pNext = NULL,
    .accelerationStructureCount = 1,
    .pAccelerationStructures = &topLevelAccelerationStructure->getAccelerationStructureHandleRef(),
  };

  descriptorSetGroup->updateDescriptorSets(
    {{0, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL, descriptorBufferInfo, NULL},
     {0, 1, 0, 1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, NULL, NULL, NULL, &writeDescriptorSetAccelerationStructure}},
    {});

  PipelineLayout* pipelineLayout = new PipelineLayout(
      device->getDeviceHandleRef(),
      {descriptorSetLayout->getDescriptorSetLayoutHandleRef()},
      {});

  auto pipelineVertexInputStateCreateInfoParam =
      std::make_shared<GraphicsPipelineGroup::
      PipelineVertexInputStateCreateInfoParam>(GraphicsPipelineGroup::
      PipelineVertexInputStateCreateInfoParam {

    .vertexInputBindingDescriptionList = {{0, sizeof(float) * 3,
        VK_VERTEX_INPUT_RATE_VERTEX}},
    .vertexInputAttributeDescriptionList = {{0, 0, VK_FORMAT_R32G32B32_SFLOAT,
        0}}
  });

  auto pipelineInputAssemblyStateCreateInfoParam =
      std::make_shared<GraphicsPipelineGroup::
      PipelineInputAssemblyStateCreateInfoParam>(GraphicsPipelineGroup::
      PipelineInputAssemblyStateCreateInfoParam {

    .primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    // .primitiveRestartEnable =
  });

  auto pipelineViewportStateCreateInfoParam =
      std::make_shared<GraphicsPipelineGroup::
      PipelineViewportStateCreateInfoParam>(GraphicsPipelineGroup::
      PipelineViewportStateCreateInfoParam {

    .viewportList = {{0, 0, 800, 600, 0, 1}},
    .scissorRect2DList = {{{0, 0}, {800, 600}}}
  });

  auto pipelineRasterizationStateCreateInfoParam =
      std::make_shared<GraphicsPipelineGroup::
      PipelineRasterizationStateCreateInfoParam>(GraphicsPipelineGroup::
      PipelineRasterizationStateCreateInfoParam {

    // .depthClampEnable = ,
    // .rasterizerDiscardEnable = ,
    // .polygonMode = ,
    // .cullModeFlags = ,
    // .frontFace = ,
    // .depthBiasEnable = ,
    // .depthBiasConstantFactor = ,
    // .depthBiasClamp = ,
    // .depthBiasSlopeFactor = ,
    .lineWidth = 1.0
  });

  auto pipelineMultisampleStateCreateInfoParam =
      std::make_shared<GraphicsPipelineGroup::
      PipelineMultisampleStateCreateInfoParam>(GraphicsPipelineGroup::
      PipelineMultisampleStateCreateInfoParam {

    .rasterizationSampleCountFlagBits = VK_SAMPLE_COUNT_1_BIT,
    // .sampleShadingEnable = ,
    // .minSampleShading = ,
    // .sampleMaskList = ,
    // .alphaToCoverageEnable = ,
    // .alphaToOneEnable =
  });

  VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {
    .blendEnable = VK_FALSE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD,
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  };

  auto pipelineColorBlendStateCreateInfoParam =
      std::make_shared<GraphicsPipelineGroup::
      PipelineColorBlendStateCreateInfoParam>(GraphicsPipelineGroup::
      PipelineColorBlendStateCreateInfoParam {

    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_COPY,
    .pipelineColorBlendAttachmentStateList =
        {pipelineColorBlendAttachmentState},
    .blendConstants = {0, 0, 0, 0}
  });

  GraphicsPipelineGroup* graphicsPipelineGroup = new GraphicsPipelineGroup(
      device->getDeviceHandleRef(),
      {
        {0,
        {vertexStage, fragmentStage},
        pipelineVertexInputStateCreateInfoParam,
        pipelineInputAssemblyStateCreateInfoParam,
        NULL,
        pipelineViewportStateCreateInfoParam,
        pipelineRasterizationStateCreateInfoParam,
        pipelineMultisampleStateCreateInfoParam,
        NULL,
        pipelineColorBlendStateCreateInfoParam,
        NULL,
        pipelineLayout->getPipelineLayoutHandleRef(),
        renderPass->getRenderPassHandleRef(),
        0,
        VK_NULL_HANDLE,
        0}});

  commandBufferGroup->beginRecording(0,
      VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

  renderPass->beginRenderPassCmd(
      commandBufferGroup->getCommandBufferHandleRef(0),
      framebuffer->getFramebufferHandleRef(),
      {{0, 0}, {800, 600}},
      {{{0.0, 0.0, 0.0, 1.0}}},
      VK_SUBPASS_CONTENTS_INLINE);

  graphicsPipelineGroup->bindPipelineCmd(0,
      commandBufferGroup->getCommandBufferHandleRef(0));

  vertexBuffer->bindVertexBufferCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), 0);
  indexBuffer->bindIndexBufferCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), VK_INDEX_TYPE_UINT32);

  descriptorSetGroup->bindDescriptorSetsCmd(
      commandBufferGroup->getCommandBufferHandleRef(0),
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipelineLayout->getPipelineLayoutHandleRef(),
      0,
      {0},
      {});
  
  graphicsPipelineGroup->drawIndexedCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), 6, 1, 0, 0, 0);

  renderPass->endRenderPassCmd(
      commandBufferGroup->getCommandBufferHandleRef(0));

  commandBufferGroup->createPipelineBarrierCmd(0,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0,
      {},
      {},
      {{VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
          VK_ACCESS_TRANSFER_READ_BIT,
          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
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

  Buffer* imageBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      800 * 600 * 4 * sizeof(float),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  commandBufferGroup->beginRecording(1,
      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  imageBuffer->copyFromImageCmd(
      commandBufferGroup->getCommandBufferHandleRef(1),
      colorImage->getImageHandleRef(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      {{0, 0, 0, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1}, {0, 0, 0},
      {800, 600, 1}}});

  commandBufferGroup->endRecording(1);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
      {{{}, {}, {1}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  void* hostImageBuffer;
  imageBuffer->mapMemory(&hostImageBuffer, 0, 800 * 600 * 4 * sizeof(float));
  saveImage("image.ppm", hostImageBuffer, 800, 600);
  imageBuffer->unmapMemory();

  return 0;
}
