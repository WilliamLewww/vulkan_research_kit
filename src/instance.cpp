#include "vrk/instance.h"

VkBool32 debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

  printf("validation layer: %s\n", pCallbackData->pMessage);

  return VK_FALSE;
}

Instance::Instance() {
  this->isActive = false;

  this->instanceHandle = VK_NULL_HANDLE;

  uint32_t apiVersion;
  VkResult result = vkEnumerateInstanceVersion(&apiVersion);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceVersion");
    exit(1);
  }

  this->majorVersion = VK_API_VERSION_MAJOR(apiVersion);
  this->minorVersion = VK_API_VERSION_MINOR(apiVersion);
  this->patchVersion = VK_API_VERSION_PATCH(apiVersion);

  this->enabledValidationFeatureList = {};
  this->disabledValidationFeatureList = {};

  this->validationFeatures = {
    .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
    .pNext = NULL,
    .enabledValidationFeatureCount = 0,
    .pEnabledValidationFeatures = NULL,
    .disabledValidationFeatureCount = 0,
    .pDisabledValidationFeatures = NULL,
  };

  this->debugUtilsMessengerCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = &this->validationFeatures,
    .flags = 0,
    .messageSeverity = 0,
    .messageType = 0,
    .pfnUserCallback = &debugCallback,
    // .pUserData =
  };

  this->applicationInfo = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = NULL,
    .pApplicationName = "Vulkan Research Kit",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = NULL,
    .engineVersion = 0,
    .apiVersion = VK_MAKE_VERSION(1, 2, 184)
  };

  uint32_t layerPropertiesCount = 0;
  result = vkEnumerateInstanceLayerProperties(&layerPropertiesCount, NULL);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceVersion");
    exit(1);
  }

  this->layerPropertiesList.resize(layerPropertiesCount);
  result = vkEnumerateInstanceLayerProperties(&layerPropertiesCount,
      this->layerPropertiesList.data());
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceVersion");
    exit(1);
  }

  uint32_t extensionPropertiesCount = 0;
  result = vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertiesCount,
      NULL);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceExtensionProperties");
    exit(1);
  }

  this->extensionPropertiesList.resize(extensionPropertiesCount);
  result = vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertiesCount,
      this->extensionPropertiesList.data());
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceExtensionProperties");
    exit(1);
  }

  this->enabledLayerNameList = {};
  this->enabledExtensionNameList = {};
}

Instance::~Instance() {
  vkDestroyInstance(this->instanceHandle, NULL);
}

std::string Instance::getVulkanVersionAPI() {
  return std::to_string(this->majorVersion) + "." + 
      std::to_string(this->minorVersion) + "." + 
      std::to_string(this->patchVersion);
}

void Instance::addValidationFeatureEnable(
    VkValidationFeatureEnableEXT validationFeatureEnable) {

  this->enabledValidationFeatureList.push_back(validationFeatureEnable);
}

void Instance::addValidationFeatureDisable(
    VkValidationFeatureDisableEXT validationFeatureDisable) {

  this->disabledValidationFeatureList.push_back(validationFeatureDisable);
}

void Instance::setDebugUtilsMessageSeverityFlagBits(
    VkDebugUtilsMessageSeverityFlagBitsEXT debugUtilsMessageSeverityFlagBits) {

  this->debugUtilsMessengerCreateInfo.messageSeverity =
      debugUtilsMessageSeverityFlagBits;
}

void Instance::setDebugUtilsMessageTypeFlagBits(
    VkDebugUtilsMessageTypeFlagBitsEXT debugUtilsMessageTypeFlagBitsEXT) {

  this->debugUtilsMessengerCreateInfo.messageType =
      debugUtilsMessageTypeFlagBitsEXT;
}

std::vector<VkLayerProperties> Instance::getAvailableLayerPropertiesList() {
  return this->layerPropertiesList;
}

bool Instance::addLayer(std::string layerName) {
  bool foundLayer = false;

  for (VkLayerProperties layerProperties : this->layerPropertiesList) {
    if (layerProperties.layerName == layerName) {
      this->enabledLayerNameList.push_back(layerName);
      foundLayer = true;
    }
  }

  return foundLayer;
}

std::vector<VkExtensionProperties> Instance::getAvailableExtensionPropertiesList
    (std::string layerName) {

  if (layerName == "") {
    return this->extensionPropertiesList;
  }

  uint32_t extensionPropertiesCountLayer = 0;
  VkResult result = vkEnumerateInstanceExtensionProperties(layerName.c_str(),
      &extensionPropertiesCountLayer, NULL);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceExtensionProperties");
    exit(1);
  }

  std::vector<VkExtensionProperties> extensionPropertiesListLayer(
      extensionPropertiesCountLayer);
  result = vkEnumerateInstanceExtensionProperties(layerName.c_str(),
      &extensionPropertiesCountLayer, extensionPropertiesListLayer.data());
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceExtensionProperties");
    exit(1);
  }

  return extensionPropertiesListLayer;
}

bool Instance::addExtension(std::string extensionName, std::string layerName) {
  bool foundExtension = false;

  if (layerName == "") {
    for (VkExtensionProperties extensionProperties :
        this->extensionPropertiesList) {
      if (extensionProperties.extensionName == extensionName) {
        this->enabledExtensionNameList.push_back(extensionName);
        foundExtension = true;
      }
    }
  }
  else {
    uint32_t extensionPropertiesCount = 0;
    VkResult result = vkEnumerateInstanceExtensionProperties(layerName.c_str(),
        &extensionPropertiesCount, NULL);
    if (result != VK_SUCCESS) {
      PRINT_RETURN_CODE(stderr, result,
          "vkEnumerateInstanceExtensionProperties");
      exit(1);
    }

    std::vector<VkExtensionProperties> extensionPropertyList(
        extensionPropertiesCount);
    result = vkEnumerateInstanceExtensionProperties(layerName.c_str(),
        &extensionPropertiesCount, extensionPropertyList.data());
    if (result != VK_SUCCESS) {
      PRINT_RETURN_CODE(stderr, result,
          "vkEnumerateInstanceExtensionProperties");
      exit(1);
    }

    for (VkExtensionProperties extensionProperties :
        extensionPropertyList) {
      if (extensionProperties.extensionName == extensionName) {
        this->enabledExtensionNameList.push_back(extensionName);
        foundExtension = true;
      }
    }
  }

  return foundExtension;
}

void Instance::activate() {
  if (this->isActive) {
    PRINT_MESSAGE(stderr, "Instance is already active");
    return;
  }

  this->validationFeatures.enabledValidationFeatureCount =
      (uint32_t)this->enabledValidationFeatureList.size();
  this->validationFeatures.pEnabledValidationFeatures =
      this->enabledValidationFeatureList.data();

  this->validationFeatures.disabledValidationFeatureCount =
      (uint32_t)this->disabledValidationFeatureList.size();
  this->validationFeatures.pDisabledValidationFeatures =
      this->disabledValidationFeatureList.data();

  const char** enabledLayerNamesUnsafe = (const char**)malloc(
      sizeof(const char*) * enabledLayerNameList.size());

  for (uint32_t x = 0; x < enabledLayerNameList.size(); x++) {
    enabledLayerNamesUnsafe[x] = enabledLayerNameList[x].c_str();
  }

  const char** enabledExtensionNamesUnsafe = (const char**)malloc(
      sizeof(const char*) * enabledExtensionNameList.size());

  for (uint32_t x = 0; x < enabledExtensionNameList.size(); x++) {
    enabledExtensionNamesUnsafe[x] = enabledExtensionNameList[x].c_str();
  }

  VkInstanceCreateInfo instanceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = &this->debugUtilsMessengerCreateInfo,
    .flags = 0,
    .pApplicationInfo = &this->applicationInfo,
    .enabledLayerCount = (uint32_t)enabledLayerNameList.size(),
    .ppEnabledLayerNames = enabledLayerNamesUnsafe,
    .enabledExtensionCount = (uint32_t)enabledExtensionNameList.size(),
    .ppEnabledExtensionNames = enabledExtensionNamesUnsafe,
  };

  VkResult result =
      vkCreateInstance(&instanceCreateInfo, NULL, &this->instanceHandle);

  free(enabledExtensionNamesUnsafe);
  free(enabledLayerNamesUnsafe);

  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkCreateInstance");
    exit(1);
  }

  this->isActive = true;
}

VkInstance Instance::getInstanceHandle() {
  return this->instanceHandle;
}