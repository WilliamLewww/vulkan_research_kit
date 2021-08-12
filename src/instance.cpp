#include "vrk/instance.h"

VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

  std::string message = pCallbackData->pMessage;

  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    message = STRING_INFO + message + STRING_RESET;
    PRINT_MESSAGE(std::cout, message.c_str());
  }

  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    message = STRING_WARNING + message + STRING_RESET;
    PRINT_MESSAGE(std::cerr, message.c_str());
  }

  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    message = STRING_ERROR + message + STRING_RESET;
    PRINT_MESSAGE(std::cerr, message.c_str());
  }

  return VK_FALSE;
}

std::vector<VkLayerProperties> Instance::getAvailableLayerPropertiesList() {
  uint32_t layerPropertiesCount = 0;

  VkResult result = vkEnumerateInstanceLayerProperties(&layerPropertiesCount,
      NULL);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumerateInstanceVersion");
  }

  std::vector<VkLayerProperties> layerPropertiesList(layerPropertiesCount);

  result = vkEnumerateInstanceLayerProperties(&layerPropertiesCount,
      layerPropertiesList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumerateInstanceVersion");
  }

  return layerPropertiesList;
}

std::vector<VkExtensionProperties>
    Instance::getAvailableExtensionPropertiesList(std::string layerName) {

  const char* layerNameConst;

  if (layerName == "") {
    layerNameConst = NULL;
  }
  else {
    layerNameConst = layerName.c_str();
  }

  uint32_t extensionPropertiesCount = 0;

  VkResult result = vkEnumerateInstanceExtensionProperties(layerNameConst,
      &extensionPropertiesCount, NULL);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumerateInstanceExtensionProperties");
  }

  std::vector<VkExtensionProperties> extensionPropertiesList(
      extensionPropertiesCount);

  result = vkEnumerateInstanceExtensionProperties(layerNameConst,
      &extensionPropertiesCount, extensionPropertiesList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumerateInstanceExtensionProperties");
  }

  return extensionPropertiesList;
}

Instance::Instance(
    std::vector<VkValidationFeatureEnableEXT> validationFeatureEnableList,
    std::vector<VkValidationFeatureDisableEXT> validationFeatureDisableList,
    VkDebugUtilsMessageSeverityFlagBitsEXT debugUtilsMessageSeverityFlagBits,
    VkDebugUtilsMessageTypeFlagBitsEXT debugUtilsMessageTypeFlagBits,
    std::string applicationName,
    uint32_t applicationVersion,
    std::vector<std::string> enabledLayerNameList,
    std::vector<std::string> enabledExtensionNameList) {

  this->instanceHandle = VK_NULL_HANDLE;

  this->debugUtilsMessengerHandle = VK_NULL_HANDLE;

  this->debugUtilsExtensionEnabled = false;

  uint32_t apiVersion;
  VkResult result = vkEnumerateInstanceVersion(&apiVersion);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkEnumerateInstanceVersion");
  }

  this->majorVersion = VK_API_VERSION_MAJOR(apiVersion);
  this->minorVersion = VK_API_VERSION_MINOR(apiVersion);
  this->patchVersion = VK_API_VERSION_PATCH(apiVersion);

  VkValidationFeaturesEXT validationFeatures = {
    .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
    .pNext = NULL,
    .enabledValidationFeatureCount =
        (uint32_t)validationFeatureEnableList.size(),
    .pEnabledValidationFeatures = validationFeatureEnableList.data(),
    .disabledValidationFeatureCount =
        (uint32_t)validationFeatureDisableList.size(),
    .pDisabledValidationFeatures = validationFeatureDisableList.data(),
  };

  VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = &validationFeatures,
    .flags = 0,
    .messageSeverity = debugUtilsMessageSeverityFlagBits,
    .messageType = debugUtilsMessageTypeFlagBits,
    .pfnUserCallback = &debugCallback,
    .pUserData = NULL
  };

  VkApplicationInfo applicationInfo = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = NULL,
    .pApplicationName = applicationName.c_str(),
    .applicationVersion = applicationVersion,
    .pEngineName = "Vulkan Research Kit",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_MAKE_VERSION(1, 2, 184)
  };

  const char** enabledLayerNameBuffer = (const char**)malloc(
      sizeof(const char*) * enabledLayerNameList.size());

  for (uint32_t x = 0; x < enabledLayerNameList.size(); x++) {
    enabledLayerNameBuffer[x] = enabledLayerNameList[x].c_str();
  }

  const char** enabledExtensionNameBuffer = (const char**)malloc(
      sizeof(const char*) * enabledExtensionNameList.size());

  for (uint32_t x = 0; x < enabledExtensionNameList.size(); x++) {
    enabledExtensionNameBuffer[x] = enabledExtensionNameList[x].c_str();
  }

  VkInstanceCreateInfo instanceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = &debugUtilsMessengerCreateInfo,
    .flags = 0,
    .pApplicationInfo = &applicationInfo,
    .enabledLayerCount = (uint32_t)enabledLayerNameList.size(),
    .ppEnabledLayerNames = enabledLayerNameBuffer,
    .enabledExtensionCount = (uint32_t)enabledExtensionNameList.size(),
    .ppEnabledExtensionNames = enabledExtensionNameBuffer,
  };

  result = vkCreateInstance(&instanceCreateInfo, NULL, &this->instanceHandle);

  free(enabledExtensionNameBuffer);
  free(enabledLayerNameBuffer);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateInstance");
  }

  if (std::find(std::begin(enabledExtensionNameList),
      std::end(enabledExtensionNameList), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) !=
      std::end(enabledExtensionNameList)) {

    this->debugUtilsExtensionEnabled = true;

    debugUtilsMessengerCreateInfo.pNext = NULL;

    LOAD_INSTANCE_FUNCTION(this->instanceHandle, vkCreateDebugUtilsMessengerEXT,
        pvkCreateDebugUtilsMessengerEXT);

    result = pvkCreateDebugUtilsMessengerEXT(this->instanceHandle,
        &debugUtilsMessengerCreateInfo, NULL, &this->debugUtilsMessengerHandle);

    if (result != VK_SUCCESS) {
      throwExceptionVulkanAPI(result, "vkCreateDebugUtilsMessengerEXT");
    }
  }
}

Instance::~Instance() {
  if (this->debugUtilsExtensionEnabled) {
    LOAD_INSTANCE_FUNCTION(this->instanceHandle,
        vkDestroyDebugUtilsMessengerEXT, pvkDestroyDebugUtilsMessengerEXT);

    pvkDestroyDebugUtilsMessengerEXT(this->instanceHandle,
        this->debugUtilsMessengerHandle, NULL);
  }

  vkDestroyInstance(this->instanceHandle, NULL);
}

std::string Instance::getVulkanVersionAPI() {
  return std::to_string(this->majorVersion) + "." +
      std::to_string(this->minorVersion) + "." +
      std::to_string(this->patchVersion);
}

VkInstance* Instance::getInstanceHandlePtr() {
  return &this->instanceHandle;
}