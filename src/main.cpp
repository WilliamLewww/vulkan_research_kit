#include "vrk/instance.h"
#include "vrk/device.h"
#include "vrk/command_pool.h"
#include "vrk/command_buffer_group.h"
#include "vrk/render_pass.h"
#include "vrk/attachment.h"
#include "vrk/shader_module.h"
#include "vrk/image.h"
#include "vrk/image_view.h"

#include <fstream>

int main() {
  Instance* instance = new Instance();
  std::cout << "Vulkan API " << instance->getVulkanVersionAPI().c_str()
      << std::endl;

  instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
  instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT);
  instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);

  instance->setDebugUtilsMessageSeverityFlagBits(
      (VkDebugUtilsMessageSeverityFlagBitsEXT)
      (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT));

  instance->setDebugUtilsMessageTypeFlagBits(
    (VkDebugUtilsMessageTypeFlagBitsEXT)
    (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT));

  instance->addLayer("VK_LAYER_KHRONOS_validation");
  instance->addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  instance->activate();

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandlePtr());

  VkPhysicalDevice activePhysicalDevice;
  for (VkPhysicalDevice deviceHandle : deviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties =
        Device::getPhysicalDeviceProperties(&deviceHandle);

    if (physicalDeviceProperties.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      activePhysicalDevice = deviceHandle;
      break;
    }
  }

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList =
      Device::getQueueFamilyPropertiesList(&activePhysicalDevice);

  uint32_t queueFamilyIndex = -1;
  for (uint32_t x = 0; x < queueFamilyPropertiesList.size(); x++) {
    if (queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndex = x;
      break;
    }
  }

  Device* device = new Device(instance->getInstanceHandlePtr(),
      &activePhysicalDevice, queueFamilyIndex, 1);
  device->activate();

  CommandPool* commandPool = new CommandPool(device->getDeviceHandlePtr(),
      queueFamilyIndex);
  commandPool->activate();

  CommandBufferGroup* commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandlePtr(), commandPool->getCommandPoolHandlePtr(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
  commandBufferGroup->activate();

  Attachment colorAttachment(VK_FORMAT_R8G8B8A8_UINT, VK_SAMPLE_COUNT_1_BIT,
    VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

  Attachment depthAttachment(VK_FORMAT_D32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
    VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
    VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

  std::vector<Attachment> colorAttachmentList = { colorAttachment };
  std::vector<VkImageLayout> colorImageLayoutList = {
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

  VkImageLayout depthImageLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  RenderPass* renderPass = new RenderPass(device->getDeviceHandlePtr(),
      VK_PIPELINE_BIND_POINT_GRAPHICS, colorAttachmentList,
      colorImageLayoutList, &depthAttachment, &depthImageLayout);
  renderPass->activate();

  std::vector<uint32_t> imageQueueFamilyIndexList = { queueFamilyIndex };

  Image* image = new Image(device->getDeviceHandlePtr(),
    &imageQueueFamilyIndexList, 0, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UINT,
    800, 600, 1, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE,
    VK_IMAGE_LAYOUT_UNDEFINED);
  image->activate();

  ImageView* imageView = new ImageView(device->getDeviceHandlePtr(), 
      image->getImageHandlePtr(), 0, VK_IMAGE_VIEW_TYPE_2D, 
      VK_FORMAT_R8G8B8A8_UINT, VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
  imageView->activate();

  std::ifstream vertexFile("resources/shaders/default.vert.spv",
      std::ios::binary | std::ios::ate);
  std::streamsize vertexFileSize = vertexFile.tellg();
  vertexFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> vertexShaderSource(vertexFileSize / sizeof(uint32_t));
  vertexFile.read((char*)vertexShaderSource.data(), vertexFileSize);
  vertexFile.close();

  ShaderModule* vertexShaderModule = new ShaderModule(
      device->getDeviceHandlePtr(), vertexShaderSource);
  vertexShaderModule->activate();

  std::ifstream fragmentFile("resources/shaders/default.vert.spv",
      std::ios::binary | std::ios::ate);
  std::streamsize fragmentFileSize = fragmentFile.tellg();
  fragmentFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> fragmentShaderSource(
      fragmentFileSize / sizeof(uint32_t));
  fragmentFile.read((char*)fragmentShaderSource.data(), fragmentFileSize);
  fragmentFile.close();

  ShaderModule* fragmentShaderModule = new ShaderModule(
      device->getDeviceHandlePtr(), fragmentShaderSource);
  fragmentShaderModule->activate();

  std::cout << *instance << std::endl;
  std::cout << std::endl;
  std::cout << *device << std::endl;
  std::cout << std::endl;
  std::cout << *commandPool << std::endl;
  std::cout << std::endl;
  std::cout << *commandBufferGroup << std::endl;
  std::cout << std::endl;
  std::cout << *renderPass << std::endl;
  std::cout << std::endl;
  std::cout << *vertexShaderModule << std::endl;
  std::cout << std::endl;
  std::cout << *fragmentShaderModule << std::endl;

  delete fragmentShaderModule;
  delete vertexShaderModule;
  delete imageView;
  delete image;
  delete renderPass;
  delete commandBufferGroup;
  delete commandPool;
  delete device;
  delete instance;

  return 0;
}
