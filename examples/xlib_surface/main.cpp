#include "vrk/instance.h"
#include "vrk/wsi/surface.h"
#include "vrk/device.h"
#include "vrk/command_pool.h"
#include "vrk/command_buffer_group.h"
#include "vrk/wsi/swapchain.h"
#include "vrk/render_pass.h"
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

#include <X11/Xlib.h>

#include <fstream>
#include <cstring>

int main(void) {
  Display* displayPtr = XOpenDisplay(NULL);
  int screen = DefaultScreen(displayPtr);

  Window window = XCreateSimpleWindow(displayPtr,
      RootWindow(displayPtr, screen), 10, 10, 100, 100, 1,
      BlackPixel(displayPtr, screen), WhitePixel(displayPtr, screen));

  XSelectInput(displayPtr, window, ExposureMask | KeyPressMask);
  XMapWindow(displayPtr, window);

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
      {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME,
          "VK_KHR_xlib_surface"});

  std::cout << "Vulkan API " << instance->getVulkanVersionAPI().c_str()
      << std::endl;

  Surface::XlibSurfaceCreateInfoParam xlibSurfaceCreateInfoParam = {
    .displayPtr = displayPtr,
    .windowPtr = std::make_shared<Window>(window)
  };

  Surface* surface = new Surface(instance->getInstanceHandleRef(),
      Surface::Platform::Xlib,
      std::make_shared<Surface::XlibSurfaceCreateInfoParam>(
          xlibSurfaceCreateInfoParam));

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
    if (queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
        Surface::checkPhysicalDeviceSurfaceSupport(activePhysicalDeviceHandle,
        x, surface->getSurfaceHandleRef())) {

      queueFamilyIndex = x;
      break;
    }
  }

  Device* device = new Device(activePhysicalDeviceHandle,
      {{0, queueFamilyIndex, 1, {1.0f}}},
      {},
      {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
      {});

  CommandPool* commandPool = new CommandPool(device->getDeviceHandleRef(),
      0,
      queueFamilyIndex);

  CommandBufferGroup* commandBufferGroup = new CommandBufferGroup(
      device->getDeviceHandleRef(), commandPool->getCommandPoolHandleRef(),
      VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      queueFamilyPropertiesList[queueFamilyIndex].queueCount);

  VkSurfaceCapabilitiesKHR surfaceCapabilities =
      surface->getPhysicalDeviceSurfaceCapabilities(activePhysicalDeviceHandle);

  std::vector<VkSurfaceFormatKHR> surfaceFormatList =
      surface->getPhysicalDeviceSurfaceFormatList(activePhysicalDeviceHandle);

  std::vector<VkPresentModeKHR> presentModeList =
      surface->getPhysicalDeviceSurfacePresentModeList(
      activePhysicalDeviceHandle);

  Swapchain* swapchain = new Swapchain(device->getDeviceHandleRef(),
      0,
      surface->getSurfaceHandleRef(),
      surfaceCapabilities.minImageCount + 1,
      surfaceFormatList[0].format,
      surfaceFormatList[0].colorSpace,
      surfaceCapabilities.currentExtent,
      1,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      surfaceCapabilities.currentTransform,
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      presentModeList[0],
      VK_TRUE,
      VK_NULL_HANDLE);

  std::vector<VkAttachmentReference> attachmentReferenceList = {
    {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
  };

  RenderPass* renderPass = new RenderPass(device->getDeviceHandleRef(),
      (VkRenderPassCreateFlagBits)0,
      {{0,
          surfaceFormatList[0].format,
          VK_SAMPLE_COUNT_1_BIT,
          VK_ATTACHMENT_LOAD_OP_CLEAR,
          VK_ATTACHMENT_STORE_OP_STORE,
          VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          VK_ATTACHMENT_STORE_OP_DONT_CARE,
          VK_IMAGE_LAYOUT_UNDEFINED,
          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}},
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

  std::vector<VkImage> imageHandleList =
      swapchain->getSwapchainImageHandleList();

  std::vector<ImageView*> imageViewList;
  std::vector<Framebuffer*> framebufferList;

  for (VkImage& imageHandle : imageHandleList) {
    imageViewList.push_back(new ImageView(
        device->getDeviceHandleRef(),
        imageHandle,
        0,
        VK_IMAGE_VIEW_TYPE_2D,
        surfaceFormatList[0].format,
        {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}));

    framebufferList.push_back(new Framebuffer(device->getDeviceHandleRef(),
        renderPass->getRenderPassHandleRef(),
        {imageViewList[imageViewList.size() - 1]->getImageViewHandleRef()},
        (VkFramebufferCreateFlags)0,
        surfaceCapabilities.currentExtent.width,
        surfaceCapabilities.currentExtent.height,
        1));
  }

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
      {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}});

  DescriptorSetLayout* descriptorSetLayout = new DescriptorSetLayout(
      device->getDeviceHandleRef(),
      0,
      {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
          NULL}});

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

  descriptorSetGroup->updateDescriptorSets(
    {{0, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL, descriptorBufferInfo, NULL}},
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

  float vertices[12] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
  };

  uint32_t indices[6] = {
    0, 1, 2, 2, 3, 0
  };

  Buffer* vertexBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      sizeof(float) * 12,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  void* hostVertexBuffer;
  vertexBuffer->mapMemory(&hostVertexBuffer, 0, 12 * sizeof(float));
  memcpy(hostVertexBuffer, vertices, 12 * sizeof(float));
  vertexBuffer->unmapMemory();

  Buffer* indexBuffer = new Buffer(device->getDeviceHandleRef(),
      activePhysicalDeviceHandle,
      0,
      sizeof(uint32_t) * 6,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE,
      {queueFamilyIndex},
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  void* hostIndexBuffer;
  indexBuffer->mapMemory(&hostIndexBuffer, 0, 6 * sizeof(uint32_t));
  memcpy(hostIndexBuffer, indices, 6 * sizeof(uint32_t));
  indexBuffer->unmapMemory();

  std::vector<Fence*> imageAvailableFenceList;
  std::vector<Semaphore*> acquireImageSemaphoreList;
  std::vector<Semaphore*> writeImageSemaphoreList;

  for (uint32_t x = 0; x < framebufferList.size(); x++) {
    imageAvailableFenceList.push_back(
        new Fence(device->getDeviceHandleRef(), VK_FENCE_CREATE_SIGNALED_BIT));
    acquireImageSemaphoreList.push_back(
        new Semaphore(device->getDeviceHandleRef(), 0));
    writeImageSemaphoreList.push_back(
        new Semaphore(device->getDeviceHandleRef(), 0));

    commandBufferGroup->beginRecording(x,
        VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

    renderPass->beginRenderPassCmd(
        commandBufferGroup->getCommandBufferHandleRef(x),
        framebufferList[x]->getFramebufferHandleRef(),
        {{0, 0}, {800, 600}},
        {{{0.0, 0.0, 0.0, 1.0}}},
        VK_SUBPASS_CONTENTS_INLINE);

    graphicsPipelineGroup->bindPipelineCmd(0,
        commandBufferGroup->getCommandBufferHandleRef(x));

    vertexBuffer->bindVertexBufferCmd(
        commandBufferGroup->getCommandBufferHandleRef(x), 0);
    indexBuffer->bindIndexBufferCmd(
        commandBufferGroup->getCommandBufferHandleRef(x), VK_INDEX_TYPE_UINT32);

    descriptorSetGroup->bindDescriptorSetsCmd(
        commandBufferGroup->getCommandBufferHandleRef(x),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout->getPipelineLayoutHandleRef(),
        0,
        {0},
        {});

    graphicsPipelineGroup->drawIndexedCmd(
        commandBufferGroup->getCommandBufferHandleRef(x), 6, 1, 0, 0, 0);

    renderPass->endRenderPassCmd(
        commandBufferGroup->getCommandBufferHandleRef(x));

    commandBufferGroup->endRecording(x);
  }

  uint32_t currentFrame = 0;
  uint32_t currentImageIndex = 0;

  XEvent event;
  while (true) {
    XNextEvent(displayPtr, &event);

    if (event.type == KeyPress) {
      if (event.xkey.keycode == 9) {
        break;
      }
      if (event.xkey.keycode == 27) {
        void* hostColorBuffer;
        colorBuffer->mapMemory(&hostColorBuffer, 0, 3 * sizeof(float));
        ((float*)hostColorBuffer)[0] += 0.1f;
        if (((float*)hostColorBuffer)[0] > 1.0f) {
          ((float*)hostColorBuffer)[0] = 0.0f;
        }
        colorBuffer->unmapMemory();
      }
      if (event.xkey.keycode == 42) {
        void* hostColorBuffer;
        colorBuffer->mapMemory(&hostColorBuffer, 0, 3 * sizeof(float));
        ((float*)hostColorBuffer)[1] += 0.1f;
        if (((float*)hostColorBuffer)[1] > 1.0f) {
          ((float*)hostColorBuffer)[1] = 0.0f;
        }
        colorBuffer->unmapMemory();
      }
      if (event.xkey.keycode == 56) {
        void* hostColorBuffer;
        colorBuffer->mapMemory(&hostColorBuffer, 0, 3 * sizeof(float));
        ((float*)hostColorBuffer)[2] += 0.1f;
        if (((float*)hostColorBuffer)[2] > 1.0f) {
          ((float*)hostColorBuffer)[2] = 0.0f;
        }
        colorBuffer->unmapMemory();
      }
    }

    imageAvailableFenceList[currentFrame]->waitForSignal(UINT32_MAX);
    imageAvailableFenceList[currentFrame]->reset();

    uint32_t currentImageIndex = 
        swapchain->aquireNextImageIndex(UINT32_MAX, 
        acquireImageSemaphoreList[currentFrame]->getSemaphoreHandleRef(), 
        VK_NULL_HANDLE);

    commandBufferGroup->submit(device->getQueueHandleRef(queueFamilyIndex, 0),
        {{{acquireImageSemaphoreList[currentFrame]->getSemaphoreHandleRef()}, 
        {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, 
        {currentImageIndex}, 
        {writeImageSemaphoreList[currentImageIndex]->getSemaphoreHandleRef()}}}, 
        imageAvailableFenceList[currentFrame]->getFenceHandleRef());

    surface->queuePresentCmd(device->getQueueHandleRef(queueFamilyIndex, 0),
        {writeImageSemaphoreList[currentImageIndex]->getSemaphoreHandleRef()},
        {swapchain->getSwapchainHandleRef()},
        {currentImageIndex},
        NULL);

    currentFrame = (currentFrame + 1) % framebufferList.size();
  }

  device->waitIdle();

  for (uint32_t x = 0; x < writeImageSemaphoreList.size(); x++) {
    delete writeImageSemaphoreList[x];
  }

  for (uint32_t x = 0; x < acquireImageSemaphoreList.size(); x++) {
    delete acquireImageSemaphoreList[x];
  }

  for (uint32_t x = 0; x < imageAvailableFenceList.size(); x++) {
    delete imageAvailableFenceList[x];
  }

  delete indexBuffer;
  delete vertexBuffer;
  delete graphicsPipelineGroup;
  delete pipelineLayout;

  delete colorBuffer;
  delete descriptorSetGroup;
  delete descriptorSetLayout;
  delete descriptorPool;

  delete fragmentShaderModule;
  delete vertexShaderModule;

  for (uint32_t x = 0; x < framebufferList.size(); x++) {
    delete framebufferList[x];
  }

  for (uint32_t x = 0; x < imageViewList.size(); x++) {
    delete imageViewList[x];
  }

  delete renderPass;
  delete swapchain;
  delete commandBufferGroup;
  delete commandPool;
  delete device;
  delete surface;
  delete instance;

  XCloseDisplay(displayPtr);

  return 0;
};