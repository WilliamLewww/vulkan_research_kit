#include "basic/engine.h"

Engine::Engine(std::string appName, bool isValidationEnabled,
               bool isRayTracingEnabled)
    : isValidationEnabled(isValidationEnabled),
      isRayTracingEnabled(isRayTracingEnabled) {

  std::vector<std::string> instanceLayerList;
  std::vector<std::string> instanceExtensionNameList;

  std::vector<VkValidationFeatureEnableEXT> validationFeatureEnableList;
  std::vector<VkValidationFeatureDisableEXT> validationFeatureDisableList;
  VkDebugUtilsMessageSeverityFlagBitsEXT debugUtilsMessageSeverityFlagBits;
  VkDebugUtilsMessageTypeFlagBitsEXT debugUtilsMessageTypeFlagBits;

  if (isValidationEnabled) {
    validationFeatureEnableList = {
        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
        VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
        VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT};

    debugUtilsMessageSeverityFlagBits =
        (VkDebugUtilsMessageSeverityFlagBitsEXT)(
            // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);

    debugUtilsMessageTypeFlagBits = (VkDebugUtilsMessageTypeFlagBitsEXT)(
        // VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);

    instanceLayerList.push_back("VK_LAYER_KHRONOS_validation");
    instanceExtensionNameList.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  if (isRayTracingEnabled) {
    instanceExtensionNameList.insert(
        instanceExtensionNameList.end(),
        RAY_TRACING_PIPELINE_REQUIRED_INSTANCE_EXTENSION_LIST.begin(),
        RAY_TRACING_PIPELINE_REQUIRED_INSTANCE_EXTENSION_LIST.end());
    instanceExtensionNameList.erase(unique(instanceExtensionNameList.begin(),
                                           instanceExtensionNameList.end()),
                                    instanceExtensionNameList.end());
  }

  instanceExtensionNameList.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  instanceExtensionNameList.push_back("VK_KHR_xlib_surface");

  this->instancePtr = std::unique_ptr<Instance>(new Instance(
      validationFeatureEnableList, validationFeatureDisableList,
      debugUtilsMessageSeverityFlagBits, debugUtilsMessageTypeFlagBits, appName,
      VK_MAKE_VERSION(1, 0, 0), instanceLayerList, instanceExtensionNameList));

  this->physicalDeviceHandleList =
      Device::getPhysicalDevices(this->instancePtr->getInstanceHandleRef());
}

Engine::~Engine() {}

std::vector<std::string> Engine::getPhysicalDeviceNameList() {
  std::vector<std::string> physicalDeviceNameList;

  for (VkPhysicalDevice physicalDeviceHandle : this->physicalDeviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties =
        Device::getPhysicalDeviceProperties(physicalDeviceHandle);

    physicalDeviceNameList.push_back(physicalDeviceProperties.deviceName);
  }

  return physicalDeviceNameList;
}

void Engine::selectWindow(Display *displayPtr,
                          std::shared_ptr<Window> windowPtr) {

  Surface::XlibSurfaceCreateInfoParam xlibSurfaceCreateInfoParam = {
      .displayPtr = displayPtr, .windowPtr = windowPtr};

  this->surfacePtr = std::unique_ptr<Surface>(new Surface(
      this->instancePtr->getInstanceHandleRef(), Surface::Platform::Xlib,
      std::make_shared<Surface::XlibSurfaceCreateInfoParam>(
          xlibSurfaceCreateInfoParam)));
}

void Engine::selectPhysicalDevice(std::string physicalDeviceName) {

  std::vector<std::string> deviceExtensionNameList;

  for (VkPhysicalDevice physicalDeviceHandle : this->physicalDeviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties =
        Device::getPhysicalDeviceProperties(physicalDeviceHandle);

    if (physicalDeviceName == physicalDeviceProperties.deviceName) {
      this->physicalDeviceHandlePtr =
          std::make_unique<VkPhysicalDevice>(physicalDeviceHandle);
    }
  }

  this->queueFamilyPropertiesList = Device::getQueueFamilyPropertiesList(
      *this->physicalDeviceHandlePtr.get());

  this->queueFamilyIndex = -1;
  for (uint32_t x = 0; x < queueFamilyPropertiesList.size(); x++) {
    if (queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
        Surface::checkPhysicalDeviceSurfaceSupport(
            *this->physicalDeviceHandlePtr.get(), x,
            surfacePtr->getSurfaceHandleRef())) {

      this->queueFamilyIndex = x;
      break;
    }
  }

  std::vector<void *> deviceCreateInfoChainList;

  deviceCreateInfoChainList.push_back(
      new VkPhysicalDeviceRobustness2FeaturesEXT(
          {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT,
           .pNext = NULL,
           .robustBufferAccess2 = VK_FALSE,
           .robustImageAccess2 = VK_FALSE,
           .nullDescriptor = VK_TRUE}));

  if (this->isRayTracingEnabled) {
    deviceCreateInfoChainList.push_back(
        new VkPhysicalDeviceBufferDeviceAddressFeatures(
            {.sType =
                 VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
             .pNext = NULL,
             .bufferDeviceAddress = VK_TRUE,
             .bufferDeviceAddressCaptureReplay = VK_FALSE,
             .bufferDeviceAddressMultiDevice = VK_FALSE}));

    deviceCreateInfoChainList.push_back(
        new VkPhysicalDeviceAccelerationStructureFeaturesKHR(
            {.sType =
                 VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
             .pNext = NULL,
             .accelerationStructure = VK_TRUE,
             .accelerationStructureCaptureReplay = VK_FALSE,
             .accelerationStructureIndirectBuild = VK_FALSE,
             .accelerationStructureHostCommands = VK_FALSE,
             .descriptorBindingAccelerationStructureUpdateAfterBind =
                 VK_FALSE}));

    deviceCreateInfoChainList.push_back(
        new VkPhysicalDeviceRayTracingPipelineFeaturesKHR(
            {.sType =
                 VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
             .pNext = NULL,
             .rayTracingPipeline = VK_TRUE,
             .rayTracingPipelineShaderGroupHandleCaptureReplay = VK_FALSE,
             .rayTracingPipelineShaderGroupHandleCaptureReplayMixed = VK_FALSE,
             .rayTracingPipelineTraceRaysIndirect = VK_FALSE,
             .rayTraversalPrimitiveCulling = VK_FALSE}));

    deviceExtensionNameList.insert(
        deviceExtensionNameList.end(),
        RAY_TRACING_PIPELINE_REQUIRED_DEVICE_EXTENSION_LIST.begin(),
        RAY_TRACING_PIPELINE_REQUIRED_DEVICE_EXTENSION_LIST.end());
    deviceExtensionNameList.erase(
        unique(deviceExtensionNameList.begin(), deviceExtensionNameList.end()),
        deviceExtensionNameList.end());
  }

  deviceExtensionNameList.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  std::shared_ptr<VkPhysicalDeviceFeatures> physicalDeviceFeaturesPtr =
      std::make_shared<VkPhysicalDeviceFeatures>(
          (VkPhysicalDeviceFeatures){.geometryShader = VK_TRUE});

  this->devicePtr = std::shared_ptr<Device>(new Device(
      *this->physicalDeviceHandlePtr.get(),
      {{0, this->queueFamilyIndex, 1, {1.0f}}}, {}, deviceExtensionNameList,
      physicalDeviceFeaturesPtr, deviceCreateInfoChainList));

  this->commandPoolPtr = std::unique_ptr<CommandPool>(new CommandPool(
      this->devicePtr->getDeviceHandleRef(),
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, this->queueFamilyIndex));

  this->commandBufferGroupPtr =
      std::unique_ptr<CommandBufferGroup>(new CommandBufferGroup(
          this->devicePtr->getDeviceHandleRef(),
          commandPoolPtr->getCommandPoolHandleRef(),
          VK_COMMAND_BUFFER_LEVEL_PRIMARY,
          queueFamilyPropertiesList[this->queueFamilyIndex].queueCount));

  this->secondaryCommandBufferCount = 128;

  this->secondaryCommandBufferGroupPtr = std::unique_ptr<CommandBufferGroup>(
      new CommandBufferGroup(this->devicePtr->getDeviceHandleRef(),
                             commandPoolPtr->getCommandPoolHandleRef(),
                             VK_COMMAND_BUFFER_LEVEL_SECONDARY,
                             this->secondaryCommandBufferCount));

  this->utilityCommandBufferCount = 3;

  this->utilityCommandBufferGroupPtr =
      std::unique_ptr<CommandBufferGroup>(new CommandBufferGroup(
          this->devicePtr->getDeviceHandleRef(),
          commandPoolPtr->getCommandPoolHandleRef(),
          VK_COMMAND_BUFFER_LEVEL_PRIMARY, this->utilityCommandBufferCount));

  this->surfaceCapabilities = surfacePtr->getPhysicalDeviceSurfaceCapabilities(
      *this->physicalDeviceHandlePtr.get());

  this->surfaceFormatList = surfacePtr->getPhysicalDeviceSurfaceFormatList(
      *this->physicalDeviceHandlePtr.get());

  this->presentModeList = surfacePtr->getPhysicalDeviceSurfacePresentModeList(
      *this->physicalDeviceHandlePtr.get());

  this->swapchainPtr = std::unique_ptr<Swapchain>(new Swapchain(
      this->devicePtr->getDeviceHandleRef(), 0,
      this->surfacePtr->getSurfaceHandleRef(),
      this->surfaceCapabilities.minImageCount + 1,
      this->surfaceFormatList[0].format, this->surfaceFormatList[0].colorSpace,
      this->surfaceCapabilities.currentExtent, 1,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE,
      {this->queueFamilyIndex}, this->surfaceCapabilities.currentTransform,
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, this->presentModeList[0], VK_TRUE,
      VK_NULL_HANDLE));

  std::vector<VkAttachmentReference> attachmentReferenceList = {
      {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
      {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};

  this->renderPassPtr = std::unique_ptr<RenderPass>(new RenderPass(
      this->devicePtr->getDeviceHandleRef(), (VkRenderPassCreateFlagBits)0,
      {{0, this->surfaceFormatList[0].format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
       {0, VK_FORMAT_D32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}},
      {{
          0,
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          0,
          NULL,
          1,
          &attachmentReferenceList[0],
          NULL,
          &attachmentReferenceList[1],
          0,
          NULL,
      }},
      {}));

  this->swapchainImageHandleList = swapchainPtr->getSwapchainImageHandleList();

  for (uint32_t x = 0; x < this->swapchainImageHandleList.size(); x++) {
    this->swapchainImageViewPtrList.push_back(
        std::unique_ptr<ImageView>(new ImageView(
            this->devicePtr->getDeviceHandleRef(),
            this->swapchainImageHandleList[x], 0, VK_IMAGE_VIEW_TYPE_2D,
            this->surfaceFormatList[0].format,
            {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
             VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1})));

    this->depthImagePtrList.push_back(std::unique_ptr<Image>(new Image(
        this->devicePtr->getDeviceHandleRef(),
        *this->physicalDeviceHandlePtr.get(), 0, VK_IMAGE_TYPE_2D,
        VK_FORMAT_D32_SFLOAT,
        {this->surfaceCapabilities.currentExtent.width,
         this->surfaceCapabilities.currentExtent.height, 1},
        1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE,
        {this->queueFamilyIndex}, VK_IMAGE_LAYOUT_UNDEFINED, 0)));

    this->depthImageViewPtrList.push_back(
        std::unique_ptr<ImageView>(new ImageView(
            this->devicePtr->getDeviceHandleRef(),
            this->depthImagePtrList[x]->getImageHandleRef(), 0,
            VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_D32_SFLOAT,
            {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
             VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1})));

    this->framebufferPtrList.push_back(
        std::unique_ptr<Framebuffer>(new Framebuffer(
            this->devicePtr->getDeviceHandleRef(),
            this->renderPassPtr->getRenderPassHandleRef(),
            {this->swapchainImageViewPtrList[x]->getImageViewHandleRef(),
             this->depthImageViewPtrList[x]->getImageViewHandleRef()},
            (VkFramebufferCreateFlags)0,
            this->surfaceCapabilities.currentExtent.width,
            this->surfaceCapabilities.currentExtent.height, 1)));

    this->imageAvailableFencePtrList.push_back(std::unique_ptr<Fence>(new Fence(
        this->devicePtr->getDeviceHandleRef(), (VkFenceCreateFlagBits)0)));
    this->acquireImageSemaphorePtrList.push_back(std::unique_ptr<Semaphore>(
        new Semaphore(this->devicePtr->getDeviceHandleRef(), 0)));
    this->writeImageSemaphorePtrList.push_back(std::unique_ptr<Semaphore>(
        new Semaphore(this->devicePtr->getDeviceHandleRef(), 0)));
  }

  for (VkImage &swapchainImageHandle : this->swapchainImageHandleList) {
  }

  this->currentFrame = 0;
}

std::shared_ptr<Scene> Engine::createScene(std::string sceneName) {
  this->scenePtrList.push_back(
      std::shared_ptr<Scene>(new Scene(sceneName, shared_from_this())));

  return this->scenePtrList[this->scenePtrList.size() - 1];
}

std::shared_ptr<Camera> Engine::createCamera(std::string cameraName) {
  this->cameraPtrList.push_back(
      std::shared_ptr<Camera>(new Camera(cameraName, shared_from_this())));

  return this->cameraPtrList[this->cameraPtrList.size() - 1];
}

uint32_t Engine::render(std::shared_ptr<Scene> scenePtr,
                        std::shared_ptr<Camera> cameraPtr) {

  for (uint32_t x = 0; x < scenePtr->getMaterialPtrList().size(); x++) {
    if (cameraPtr->getIsCameraBufferDirty()) {
      scenePtr->getMaterialPtrList()[x]->updateCameraDescriptorSet(cameraPtr);
    }
    for (uint32_t y = 0; y < scenePtr->getLightPtrList().size(); y++) {
      if (scenePtr->getLightPtrList()[y]->getIsLightBufferDirty()) {
        scenePtr->getMaterialPtrList()[x]->updateLightDescriptorSet(
            scenePtr->getLightPtrList()[y]);
      }
    }
    for (uint32_t y = 0; y < scenePtr->getModelPtrList().size(); y++) {
      if (scenePtr->getModelPtrList()[y]->getIsModelBufferDirty()) {
        scenePtr->getMaterialPtrList()[x]->updateModelDescriptorSet(
            scenePtr->getModelPtrList()[y]);
      }
    }
    scenePtr->getMaterialPtrList()[x]->updateSceneDescriptorSet(scenePtr);
  }
  if (cameraPtr->getIsCameraBufferDirty()) {
    cameraPtr->resetIsCameraBufferDirty();
  }
  for (uint32_t x = 0; x < scenePtr->getLightPtrList().size(); x++) {
    if (scenePtr->getLightPtrList()[x]->getIsLightBufferDirty()) {
      scenePtr->getLightPtrList()[x]->resetIsLightBufferDirty();
    }
  }
  for (uint32_t x = 0; x < scenePtr->getModelPtrList().size(); x++) {
    if (scenePtr->getModelPtrList()[x]->getIsModelBufferDirty()) {
      scenePtr->getModelPtrList()[x]->resetIsModelBufferDirty();
    }
  }
  scenePtr->recordCommandBuffer(this->currentFrame);

  uint32_t currentImageIndex = this->swapchainPtr->aquireNextImageIndex(
      UINT32_MAX,
      this->acquireImageSemaphorePtrList[this->currentFrame]
          ->getSemaphoreHandleRef(),
      VK_NULL_HANDLE);

  this->commandBufferGroupPtr->submit(
      this->devicePtr->getQueueHandleRef(this->queueFamilyIndex, 0),
      {{{this->acquireImageSemaphorePtrList[this->currentFrame]
             ->getSemaphoreHandleRef()},
        {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        {currentImageIndex},
        {this->writeImageSemaphorePtrList[currentImageIndex]
             ->getSemaphoreHandleRef()}}},
      this->imageAvailableFencePtrList[this->currentFrame]
          ->getFenceHandleRef());

  this->surfacePtr->queuePresentCmd(
      this->devicePtr->getQueueHandleRef(this->queueFamilyIndex, 0),
      {this->writeImageSemaphorePtrList[currentImageIndex]
           ->getSemaphoreHandleRef()},
      {this->swapchainPtr->getSwapchainHandleRef()}, {currentImageIndex}, NULL);

  this->imageAvailableFencePtrList[this->currentFrame]->waitForSignal(
      UINT32_MAX);
  this->imageAvailableFencePtrList[this->currentFrame]->reset();

  this->currentFrame =
      (this->currentFrame + 1) % this->framebufferPtrList.size();
  return this->currentFrame;
}
