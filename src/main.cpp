#include "vrk/instance.h"
#include "vrk/device.h"
#include "vrk/ray_tracing/acceleration_structure.h"

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

  Device* device = new Device(activePhysicalDeviceHandle,
      {{0, queueFamilyIndex, 1, {1.0f}}},
      {},
      {"VK_KHR_acceleration_structure", "VK_KHR_deferred_host_operations"},
      NULL);

  AccelerationStructure::AccelerationStructureGeometryDataParam
      accelerationStructureGeometryDataParam = {

    .accelerationStructureGeometryTrianglesDataParam = {
      .vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
      .vertexData = {
        .deviceAddress = 0
      },
      .vertexStride = sizeof(float) * 3,
      .maxVertex = 3,
      .indexType = VK_INDEX_TYPE_UINT32,
      .indexData = {
        .deviceAddress = 0
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
      VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_KHR,
      VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
      0,
      {{VK_GEOMETRY_TYPE_TRIANGLES_KHR,
          accelerationStructureGeometryDataParam,
          0}},
      {1});

  printf("%d %d %d\n", accelerationStructureBuildSizesInfoParam.accelerationStructureSize,
      accelerationStructureBuildSizesInfoParam.updateScratchSize,
      accelerationStructureBuildSizesInfoParam.buildScratchSize);

  // AccelerationStructure* accelerationStructure = new AccelerationStructure(
  //     device->getDeviceHandleRef(),
  //     0);

  return 0;
}