#include "vrk/instance.h"
#include "vrk/device.h"
#include "vrk/buffer.h"
#include "vrk/command_pool.h"
#include "vrk/command_buffer_group.h"
#include "vrk/fence.h"

#include "vrk/ray_tracing/acceleration_structure.h"

#include <cstring>

int main(void) {
  std::vector<VkValidationFeatureEnableEXT> validationFeatureEnableList = {
    VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
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
      {VK_EXT_DEBUG_UTILS_EXTENSION_NAME});

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
    .bufferDeviceAddress = true,
    .bufferDeviceAddressCaptureReplay = false,
    .bufferDeviceAddressMultiDevice = false
  };

  VkPhysicalDeviceAccelerationStructureFeaturesKHR
      physicalDeviceAccelerationStructureFeatures = {

    .sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
    .pNext = NULL,
    .accelerationStructure = true,
    .accelerationStructureCaptureReplay = false,
    .accelerationStructureIndirectBuild = false,
    .accelerationStructureHostCommands = false,
    .descriptorBindingAccelerationStructureUpdateAfterBind = false
  };

  VkPhysicalDeviceRayQueryFeaturesKHR
      physicalDeviceRayQueryFeatures = {

    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR,
    .pNext = NULL,
    .rayQuery = true
  };

  Device* device = new Device(activePhysicalDeviceHandle,
      {{0, queueFamilyIndex, 1, {1.0f}}},
      {},
      {"VK_KHR_acceleration_structure", "VK_KHR_deferred_host_operations",
          "VK_KHR_buffer_device_address"},
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
  memcpy(hostGeometryInstanceBuffer, vertices, sizeof(VkAccelerationStructureInstanceKHR));
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

  return 0;
}
