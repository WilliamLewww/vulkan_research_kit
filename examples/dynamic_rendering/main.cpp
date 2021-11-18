#include "vrk/buffer.h"
#include "vrk/command_buffer_group.h"
#include "vrk/command_pool.h"
#include "vrk/device.h"
#include "vrk/fence.h"
#include "vrk/graphics_pipeline_group.h"
#include "vrk/image.h"
#include "vrk/image_view.h"
#include "vrk/instance.h"
#include "vrk/pipeline_layout.h"
#include "vrk/resource.h"
#include "vrk/shader_module.h"

#include "vrk/dynamic_rendering/dynamic_rendering.h"

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

int main() {
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
      DYNAMIC_RENDERING_REQUIRED_INSTANCE_EXTENSION_LIST.begin(),
      DYNAMIC_RENDERING_REQUIRED_INSTANCE_EXTENSION_LIST.end());
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

  VkPhysicalDeviceDynamicRenderingFeaturesKHR
      physicalDeviceDynamicRenderingFeatures = {
          .sType =
              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
          .pNext = NULL,
          .dynamicRendering = VK_TRUE};

  std::vector<std::string> deviceExtensionList;
  deviceExtensionList.insert(
      deviceExtensionList.end(),
      DYNAMIC_RENDERING_REQUIRED_DEVICE_EXTENSION_LIST.begin(),
      DYNAMIC_RENDERING_REQUIRED_DEVICE_EXTENSION_LIST.end());
  deviceExtensionList.erase(
      unique(deviceExtensionList.begin(), deviceExtensionList.end()),
      deviceExtensionList.end());

  Device *device = new Device(
      activePhysicalDeviceHandle, {{0, queueFamilyIndex, 1, {1.0f}}}, {},
      deviceExtensionList, NULL, {&physicalDeviceDynamicRenderingFeatures});

  CommandPool *commandPool =
      new CommandPool(device->getDeviceHandleRef(), 0, queueFamilyIndex);

  CommandBufferGroup *commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandleRef(), commandPool->getCommandPoolHandleRef(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2);

  std::ifstream vertexFile(Resource::findResource("shaders/default.vert.spv"),
                           std::ios::binary | std::ios::ate);
  std::streamsize vertexFileSize = vertexFile.tellg();
  vertexFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> vertexShaderSource(vertexFileSize / sizeof(uint32_t));
  vertexFile.read((char *)vertexShaderSource.data(), vertexFileSize);
  vertexFile.close();

  ShaderModule *vertexShaderModule =
      new ShaderModule(device->getDeviceHandleRef(), vertexShaderSource);

  std::ifstream fragmentFile(Resource::findResource("shaders/default.frag.spv"),
                             std::ios::binary | std::ios::ate);
  std::streamsize fragmentFileSize = fragmentFile.tellg();
  fragmentFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> fragmentShaderSource(fragmentFileSize /
                                             sizeof(uint32_t));
  fragmentFile.read((char *)fragmentShaderSource.data(), fragmentFileSize);
  fragmentFile.close();

  ShaderModule *fragmentShaderModule =
      new ShaderModule(device->getDeviceHandleRef(), fragmentShaderSource);

  GraphicsPipelineGroup::PipelineShaderStageCreateInfoParam vertexStage = {
      .pipelineShaderStageCreateFlags = 0,
      .shaderStageFlagBits = VK_SHADER_STAGE_VERTEX_BIT,
      .shaderModuleHandleRef = vertexShaderModule->getShaderModuleHandleRef(),
      .entryPointName = "main",
      .specializationInfoPtr = NULL};

  GraphicsPipelineGroup::PipelineShaderStageCreateInfoParam fragmentStage = {
      .pipelineShaderStageCreateFlags = 0,
      .shaderStageFlagBits = VK_SHADER_STAGE_FRAGMENT_BIT,
      .shaderModuleHandleRef = fragmentShaderModule->getShaderModuleHandleRef(),
      .entryPointName = "main",
      .specializationInfoPtr = NULL};

  PipelineLayout *pipelineLayout = new PipelineLayout(
      device->getDeviceHandleRef(), std::vector<VkDescriptorSetLayout>(),
      std::vector<VkPushConstantRange>());

  auto pipelineVertexInputStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineVertexInputStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineVertexInputStateCreateInfoParam{

          .vertexInputBindingDescriptionList = {{0, sizeof(float) * 3,
                                                 VK_VERTEX_INPUT_RATE_VERTEX}},
          .vertexInputAttributeDescriptionList = {
              {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0}}});

  auto pipelineInputAssemblyStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineInputAssemblyStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineInputAssemblyStateCreateInfoParam{

          .primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
          // .primitiveRestartEnable =
      });

  auto pipelineViewportStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineViewportStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineViewportStateCreateInfoParam{

          .viewportList = {{0, 0, 800, 600, 0, 1}},
          .scissorRect2DList = {{{0, 0}, {800, 600}}}});

  auto pipelineRasterizationStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineRasterizationStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineRasterizationStateCreateInfoParam{

          // .depthClampEnable = ,
          // .rasterizerDiscardEnable = ,
          // .polygonMode = ,
          // .cullModeFlags = ,
          // .frontFace = ,
          // .depthBiasEnable = ,
          // .depthBiasConstantFactor = ,
          // .depthBiasClamp = ,
          // .depthBiasSlopeFactor = ,
          .lineWidth = 1.0});

  auto pipelineMultisampleStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineMultisampleStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineMultisampleStateCreateInfoParam{

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
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

  auto pipelineColorBlendStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineColorBlendStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineColorBlendStateCreateInfoParam{

          .logicOpEnable = VK_FALSE,
          .logicOp = VK_LOGIC_OP_COPY,
          .pipelineColorBlendAttachmentStateList =
              {pipelineColorBlendAttachmentState},
          .blendConstants = {0, 0, 0, 0}});

  std::vector<VkFormat> colorAttachmentFormatList = {
      VK_FORMAT_R32G32B32A32_SFLOAT};

  VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
      .pNext = NULL,
      .viewMask = 0,
      .colorAttachmentCount = 1,
      .pColorAttachmentFormats = colorAttachmentFormatList.data(),
      .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
      .stencilAttachmentFormat = VK_FORMAT_UNDEFINED};

  GraphicsPipelineGroup *graphicsPipelineGroup =
      new GraphicsPipelineGroup(device->getDeviceHandleRef(),
                                {{0,
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
                                  VK_NULL_HANDLE,
                                  0,
                                  VK_NULL_HANDLE,
                                  0,
                                  {&pipelineRenderingCreateInfo}}});

  float vertices[12] = {-0.5f, -0.5f, 0.0f, 0.5f,  -0.5f, 0.0f,
                        0.5f,  0.5f,  0.0f, -0.5f, 0.5f,  0.0f};

  uint32_t indices[6] = {0, 1, 2, 2, 3, 0};

  Buffer *vertexBuffer =
      new Buffer(device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
                 sizeof(float) * 12, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  void *hostVertexBuffer;
  vertexBuffer->mapMemory(&hostVertexBuffer, 0, 12 * sizeof(float));
  memcpy(hostVertexBuffer, vertices, 12 * sizeof(float));
  vertexBuffer->unmapMemory();

  Buffer *indexBuffer =
      new Buffer(device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
                 sizeof(uint32_t) * 6, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  void *hostIndexBuffer;
  indexBuffer->mapMemory(&hostIndexBuffer, 0, 6 * sizeof(uint32_t));
  memcpy(hostIndexBuffer, indices, 6 * sizeof(uint32_t));
  indexBuffer->unmapMemory();

  Image *colorImage = new Image(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      VK_IMAGE_TYPE_2D, VK_FORMAT_R32G32B32A32_SFLOAT, {800, 600, 1}, 1, 1,
      VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex}, VK_IMAGE_LAYOUT_UNDEFINED,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  ImageView *colorImageView = new ImageView(
      device->getDeviceHandleRef(), colorImage->getImageHandleRef(), 0,
      VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32G32B32A32_SFLOAT,
      {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
       VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
      {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

  DynamicRendering::RenderingAttachmentInfoParam renderingAttachmentInfoParam =
      {.imageViewHandle = colorImageView->getImageViewHandleRef(),
       .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
       .resolveModeFlagBits = (VkResolveModeFlagBits)0,
       .resolveImageViewHandle = VK_NULL_HANDLE,
       .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
       .attachmentLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
       .attachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
       .clearValue = {.color = {{0, 0, 0, 0}}}};

  commandBufferGroup->beginRecording(
      0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  DynamicRendering::beginRenderingCmd(
      device->getDeviceHandleRef(),
      commandBufferGroup->getCommandBufferHandleRef(0), 0, {{0, 0}, {800, 600}},
      1, 0, 1, {renderingAttachmentInfoParam}, NULL, NULL);

  graphicsPipelineGroup->bindPipelineCmd(
      0, commandBufferGroup->getCommandBufferHandleRef(0));

  vertexBuffer->bindVertexBufferCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), 0);
  indexBuffer->bindIndexBufferCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), VK_INDEX_TYPE_UINT32);

  graphicsPipelineGroup->drawIndexedCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), 6, 1, 0, 0, 0);

  DynamicRendering::endRenderingCmd(
      device->getDeviceHandleRef(),
      commandBufferGroup->getCommandBufferHandleRef(0));

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

  Fence *fence =
      new Fence(device->getDeviceHandleRef(), (VkFenceCreateFlagBits)0);

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

  return 0;
}
