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
      this->devicePtr->getDeviceHandleRef(), 0, this->queueFamilyIndex));

  this->commandBufferGroupPtr =
      std::unique_ptr<CommandBufferGroup>(new CommandBufferGroup(
          this->devicePtr->getDeviceHandleRef(),
          commandPoolPtr->getCommandPoolHandleRef(),
          VK_COMMAND_BUFFER_LEVEL_PRIMARY,
          queueFamilyPropertiesList[this->queueFamilyIndex].queueCount));

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
  }
}

std::shared_ptr<Material> Engine::createMaterial(std::string materialName,
                                                 std::string vertexFileName,
                                                 std::string fragmentFileName) {

  this->materialPtrList.push_back(std::shared_ptr<Material>(new Material(
      shared_from_this(), materialName, vertexFileName, fragmentFileName)));

  return this->materialPtrList[this->materialPtrList.size() - 1];
}

std::shared_ptr<Model>
Engine::createModel(std::string modelName, std::string modelPath,
                    std::shared_ptr<Material> materialPtr) {

  this->modelPtrList.push_back(std::shared_ptr<Model>(
      new Model(shared_from_this(), modelName, modelPath, materialPtr)));

  return this->modelPtrList[this->modelPtrList.size() - 1];
}
