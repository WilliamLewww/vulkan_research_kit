#include "vrk/instance.h"
#include "vrk/device.h"
#include "vrk/command_pool.h"
#include "vrk/command_buffer_group.h"
#include "vrk/render_pass.h"
#include "vrk/framebuffer.h"
#include "vrk/shader_module.h"

#include <fstream>

int main(void) {
  Instance* instance = new Instance(
      {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
          VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
          VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT},
      {},
      (VkDebugUtilsMessageSeverityFlagBitsEXT)
          (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT),
      (VkDebugUtilsMessageTypeFlagBitsEXT)
          (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT),
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
      {},
      {});

  CommandPool* commandPool = new CommandPool(device->getDeviceHandleRef(),
      0,
      queueFamilyIndex);

  CommandBufferGroup* commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandleRef(), commandPool->getCommandPoolHandleRef(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

  RenderPass* renderPass = new RenderPass(device->getDeviceHandleRef(),
      (VkRenderPassCreateFlagBits)0,
      {},
      {{0,
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          0,
          NULL,
          0,
          NULL,
          NULL,
          NULL,
          0,
          NULL,
      }},
      {});

  Framebuffer* framebuffer = new Framebuffer(device->getDeviceHandleRef(),
      renderPass->getRenderPassHandleRef(),
      std::vector<VkImageView>(),
      (VkFramebufferCreateFlags)0,
      800,
      600,
      1);

  std::ifstream vertexFile("resources/shaders/default.vert.spv",
      std::ios::binary | std::ios::ate);
  std::streamsize vertexFileSize = vertexFile.tellg();
  vertexFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> vertexShaderSource(vertexFileSize / sizeof(uint32_t));
  vertexFile.read((char*)vertexShaderSource.data(), vertexFileSize);
  vertexFile.close();

  ShaderModule* vertexShaderModule = new ShaderModule(
      device->getDeviceHandleRef(), vertexShaderSource);

  std::ifstream fragmentFile("resources/shaders/default.vert.spv",
      std::ios::binary | std::ios::ate);
  std::streamsize fragmentFileSize = fragmentFile.tellg();
  fragmentFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> fragmentShaderSource(
      fragmentFileSize / sizeof(uint32_t));
  fragmentFile.read((char*)fragmentShaderSource.data(), fragmentFileSize);
  fragmentFile.close();

  ShaderModule* fragmentShaderModule = new ShaderModule(
      device->getDeviceHandleRef(), fragmentShaderSource);

  delete fragmentShaderModule;
  delete vertexShaderModule;

  delete framebuffer;
  delete renderPass;
  delete commandBufferGroup;
  delete commandPool;
  delete device;
  delete instance;

  return 0;
}