#include "basic/engine.h"

Engine::Engine(std::string appName, bool enableValidation,
               std::vector<std::string> instanceLayerList,
               std::vector<std::string> instanceExtensionNameList) {

  std::vector<VkValidationFeatureEnableEXT> validationFeatureEnableList;
  std::vector<VkValidationFeatureDisableEXT> validationFeatureDisableList;
  VkDebugUtilsMessageSeverityFlagBitsEXT debugUtilsMessageSeverityFlagBits;
  VkDebugUtilsMessageTypeFlagBitsEXT debugUtilsMessageTypeFlagBits;

  if (enableValidation) {
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

void Engine::selectPhysicalDevice(
    std::string physicalDeviceName,
    std::vector<std::string> deviceExtensionNameList) {

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

  deviceExtensionNameList.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  this->devicePtr = std::shared_ptr<Device>(
      new Device(*this->physicalDeviceHandlePtr.get(),
                 {{0, this->queueFamilyIndex, 1, {1.0f}}}, {},
                 deviceExtensionNameList, NULL));

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
      {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}};

  this->renderPassPtr = std::unique_ptr<RenderPass>(new RenderPass(
      this->devicePtr->getDeviceHandleRef(), (VkRenderPassCreateFlagBits)0,
      {{0, this->surfaceFormatList[0].format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}},
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
      {}));

  this->swapchainImageHandleList = swapchainPtr->getSwapchainImageHandleList();

  for (VkImage &swapchainImageHandle : this->swapchainImageHandleList) {
    this->swapchainImageViewPtrList.push_back(
        std::unique_ptr<ImageView>(new ImageView(
            this->devicePtr->getDeviceHandleRef(), swapchainImageHandle, 0,
            VK_IMAGE_VIEW_TYPE_2D, this->surfaceFormatList[0].format,
            {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
             VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1})));

    this->framebufferPtrList.push_back(std::unique_ptr<Framebuffer>(
        new Framebuffer(this->devicePtr->getDeviceHandleRef(),
                        this->renderPassPtr->getRenderPassHandleRef(),
                        {this->swapchainImageViewPtrList
                             [this->swapchainImageViewPtrList.size() - 1]
                                 ->getImageViewHandleRef()},
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
      cameraPtr->resetIsCameraBufferDirty();
    }
    for (uint32_t y = 0; y < scenePtr->getLightPtrList().size(); y++) {
      if (scenePtr->getLightPtrList()[x]->getIsLightBufferDirty()) {
        scenePtr->getMaterialPtrList()[x]->updateLightDescriptorSet(
            scenePtr->getLightPtrList()[y]);
        scenePtr->getLightPtrList()[x]->resetIsLightBufferDirty();
      }
    }
    scenePtr->getMaterialPtrList()[x]->updateSceneDescriptorSet(scenePtr);
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
