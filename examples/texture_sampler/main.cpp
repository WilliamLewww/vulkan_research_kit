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
#include "vrk/sampler.h"
#include "vrk/shader_module.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cstring>
#include <fstream>

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

  CommandPool *commandPool = new CommandPool(
      device->getDeviceHandleRef(),
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueFamilyIndex);

  CommandBufferGroup *commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandleRef(), commandPool->getCommandPoolHandleRef(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2);

  std::vector<VkAttachmentReference> attachmentReferenceList = {
      {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};

  RenderPass *renderPass = new RenderPass(
      device->getDeviceHandleRef(), (VkRenderPassCreateFlagBits)0,
      {{0, VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}},
      {{
          0,
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

  Framebuffer *framebuffer = new Framebuffer(
      device->getDeviceHandleRef(), renderPass->getRenderPassHandleRef(),
      {colorImageView->getImageViewHandleRef()}, (VkFramebufferCreateFlags)0,
      800, 600, 1);

  std::ifstream vertexFile(
      Resource::findResource("resources/shaders/shader.vert.spv"),
      std::ios::binary | std::ios::ate);
  std::streamsize vertexFileSize = vertexFile.tellg();
  vertexFile.seekg(0, std::ios::beg);
  std::vector<uint32_t> vertexShaderSource(vertexFileSize / sizeof(uint32_t));
  vertexFile.read((char *)vertexShaderSource.data(), vertexFileSize);
  vertexFile.close();

  ShaderModule *vertexShaderModule =
      new ShaderModule(device->getDeviceHandleRef(), vertexShaderSource);

  std::ifstream fragmentFile(
      Resource::findResource("resources/shaders/shader.frag.spv"),
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

  DescriptorPool *descriptorPool = new DescriptorPool(
      device->getDeviceHandleRef(),
      VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1,
      {{VK_DESCRIPTOR_TYPE_SAMPLER, 1}, {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1}});

  DescriptorSetLayout *descriptorSetLayout = new DescriptorSetLayout(
      device->getDeviceHandleRef(), 0,
      {{0, VK_DESCRIPTOR_TYPE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, NULL},
       {1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
        NULL}});

  DescriptorSetGroup *descriptorSetGroup = new DescriptorSetGroup(
      device->getDeviceHandleRef(),
      descriptorPool->getDescriptorPoolHandleRef(),
      {descriptorSetLayout->getDescriptorSetLayoutHandleRef()});

  Sampler *sampler = new Sampler(
      device->getDeviceHandleRef(), 0, VK_FILTER_NEAREST, VK_FILTER_NEAREST,
      VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT,
      VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0,
      VK_FALSE, 0, VK_FALSE, VK_COMPARE_OP_NEVER, 0, 0,
      VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, VK_FALSE);

  Image *image =
      new Image(device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
                VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, {250, 250, 1}, 1, 1,
                VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
                VK_IMAGE_LAYOUT_UNDEFINED, 0);

  int texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load("resources/textures/texture.jpg", &texWidth,
                              &texHeight, &texChannels, STBI_rgb_alpha);
  VkDeviceSize imageSize = texWidth * texHeight * 4;

  Buffer *imageBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0, imageSize,
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  void *hostImageBuffer;
  imageBuffer->mapMemory(&hostImageBuffer, 0, imageSize);
  memcpy(hostImageBuffer, pixels, imageSize);
  imageBuffer->unmapMemory();

  ImageView *imageView = new ImageView(
      device->getDeviceHandleRef(), image->getImageHandleRef(), 0,
      VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB,
      {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
       VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
      {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

  Fence *fence =
      new Fence(device->getDeviceHandleRef(), (VkFenceCreateFlagBits)0);

  commandBufferGroup->beginRecording(
      1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  commandBufferGroup->createPipelineBarrierCmd(
      1, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, {}, {},
      {{VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        queueFamilyIndex,
        queueFamilyIndex,
        image->getImageHandleRef(),
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

  commandBufferGroup->endRecording(1);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
                             {{{}, {}, {1}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  commandBufferGroup->beginRecording(
      1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  image->copyFromBufferCmd(commandBufferGroup->getCommandBufferHandleRef(1),
                           imageBuffer->getBufferHandleRef(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           {{0,
                             0,
                             0,
                             {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                             {0, 0, 0},
                             {250, 250, 1}}});

  commandBufferGroup->endRecording(1);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
                             {{{}, {}, {1}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  commandBufferGroup->beginRecording(
      1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  commandBufferGroup->createPipelineBarrierCmd(
      1, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0, {}, {},
      {{VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        queueFamilyIndex,
        queueFamilyIndex,
        image->getImageHandleRef(),
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}}});

  commandBufferGroup->endRecording(1);

  commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
                             {{{}, {}, {1}, {}}}, fence->getFenceHandleRef());

  fence->waitForSignal(UINT32_MAX);
  fence->reset();

  VkDescriptorImageInfo descriptorSamplerImageInfo = {
      .sampler = sampler->getSamplerHandleRef(),
      .imageView = VK_NULL_HANDLE,
      .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED};

  VkDescriptorImageInfo descriptorImageInfo = {
      .sampler = VK_NULL_HANDLE,
      .imageView = imageView->getImageViewHandleRef(),
      .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

  descriptorSetGroup->updateDescriptorSets({{0,
                                             0,
                                             0,
                                             1,
                                             VK_DESCRIPTOR_TYPE_SAMPLER,
                                             {descriptorSamplerImageInfo},
                                             {},
                                             {}},
                                            {0,
                                             1,
                                             0,
                                             1,
                                             VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                             {descriptorImageInfo},
                                             {},
                                             {}}},
                                           {});

  PipelineLayout *pipelineLayout = new PipelineLayout(
      device->getDeviceHandleRef(),
      {descriptorSetLayout->getDescriptorSetLayoutHandleRef()},
      std::vector<VkPushConstantRange>());

  auto pipelineVertexInputStateCreateInfoParam = std::make_shared<
      GraphicsPipelineGroup::PipelineVertexInputStateCreateInfoParam>(
      GraphicsPipelineGroup::PipelineVertexInputStateCreateInfoParam{

          .vertexInputBindingDescriptionList = {{0, sizeof(float) * 5,
                                                 VK_VERTEX_INPUT_RATE_VERTEX}},
          .vertexInputAttributeDescriptionList = {
              {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
              {1, 0, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 3}}});

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
                                  renderPass->getRenderPassHandleRef(),
                                  0,
                                  VK_NULL_HANDLE,
                                  0}});

  float vertices[20] = {-0.5f, -0.5f, 0.0f, 0.0,  0.0,  0.5f, -0.5f,
                        0.0f,  1.0,   0.0,  0.5f, 0.5f, 0.0f, 1.0,
                        1.0,   -0.5f, 0.5f, 0.0f, 0.0,  1.0};

  uint32_t indices[6] = {0, 1, 2, 2, 3, 0};

  Buffer *vertexBuffer =
      new Buffer(device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
                 sizeof(float) * 20, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  void *hostVertexBuffer;
  vertexBuffer->mapMemory(&hostVertexBuffer, 0, 20 * sizeof(float));
  memcpy(hostVertexBuffer, vertices, 20 * sizeof(float));
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

  commandBufferGroup->beginRecording(
      0, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

  renderPass->beginRenderPassCmd(
      commandBufferGroup->getCommandBufferHandleRef(0),
      framebuffer->getFramebufferHandleRef(), {{0, 0}, {800, 600}},
      {{{0.0, 0.0, 0.0, 1.0}}}, VK_SUBPASS_CONTENTS_INLINE);

  graphicsPipelineGroup->bindPipelineCmd(
      0, commandBufferGroup->getCommandBufferHandleRef(0));

  vertexBuffer->bindVertexBufferCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), 0);
  indexBuffer->bindIndexBufferCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), VK_INDEX_TYPE_UINT32);

  descriptorSetGroup->bindDescriptorSetsCmd(
      commandBufferGroup->getCommandBufferHandleRef(0),
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipelineLayout->getPipelineLayoutHandleRef(), 0, {0}, {});

  graphicsPipelineGroup->drawIndexedCmd(
      commandBufferGroup->getCommandBufferHandleRef(0), 6, 1, 0, 0, 0);

  renderPass->endRenderPassCmd(
      commandBufferGroup->getCommandBufferHandleRef(0));

  commandBufferGroup->createPipelineBarrierCmd(
      0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, {}, {},
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

  Buffer *imageOutBuffer = new Buffer(
      device->getDeviceHandleRef(), activePhysicalDeviceHandle, 0,
      800 * 600 * 4 * sizeof(float),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_SHARING_MODE_EXCLUSIVE, {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  commandBufferGroup->beginRecording(
      1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

  imageOutBuffer->copyFromImageCmd(
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

  void *hostImageOutBuffer;
  imageOutBuffer->mapMemory(&hostImageOutBuffer, 0,
                            800 * 600 * 4 * sizeof(float));
  saveImage("image.ppm", hostImageOutBuffer, 800, 600);
  imageOutBuffer->unmapMemory();

  delete imageOutBuffer;
  delete indexBuffer;
  delete vertexBuffer;
  delete graphicsPipelineGroup;
  delete pipelineLayout;
  delete fence;
  delete imageView;
  delete imageBuffer;
  delete image;
  delete sampler;
  delete descriptorSetGroup;
  delete descriptorSetLayout;
  delete descriptorPool;
  delete fragmentShaderModule;
  delete vertexShaderModule;
  delete framebuffer;
  delete colorImageView;
  delete colorImage;
  delete renderPass;
  delete commandBufferGroup;
  delete commandPool;
  delete device;
  delete instance;

  return 0;
}
