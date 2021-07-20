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

  uint32_t apiVersion;
  VkResult result = vkEnumerateInstanceVersion(&apiVersion);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceVersion");
    exit(1);
  }

  this->majorVersion = VK_API_VERSION_MAJOR(apiVersion);
  this->minorVersion = VK_API_VERSION_MINOR(apiVersion);
  this->patchVersion = VK_API_VERSION_PATCH(apiVersion);

  this->instance = VK_NULL_HANDLE;

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

  this->layerPropertyCount = 0;
  result = vkEnumerateInstanceLayerProperties(&this->layerPropertyCount, NULL);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceVersion");
    exit(1);
  }

  this->layerPropertyList.resize(this->layerPropertyCount);
  result = vkEnumerateInstanceLayerProperties(&this->layerPropertyCount,
      this->layerPropertyList.data());
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceVersion");
    exit(1);
  }

  this->enabledLayerNameList = {};

  this->extensionPropertyCount = 0;
  result = vkEnumerateInstanceExtensionProperties(NULL,
      &this->extensionPropertyCount, NULL);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceExtensionProperties");
    exit(1);
  }

  this->extensionPropertyList.resize(this->extensionPropertyCount);
  result = vkEnumerateInstanceExtensionProperties(NULL,
      &this->extensionPropertyCount, this->extensionPropertyList.data());
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceExtensionProperties");
    exit(1);
  }

  this->enabledExtensionNameList = {};
}

Instance::~Instance() {
  vkDestroyInstance(this->instance, NULL);
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
    VkDebugUtilsMessageTypeFlagBitsEXT
    debugUtilsMessageTypeFlagBitsEXT) {

  this->debugUtilsMessengerCreateInfo.messageType =
      debugUtilsMessageTypeFlagBitsEXT;
}

std::vector<VkLayerProperties> Instance::getAvailableLayers() {
  return this->layerPropertyList;
}

bool Instance::addLayer(std::string layerName) {
  bool foundLayer = false;

  for (VkLayerProperties layerProperties : this->layerPropertyList) {
    if (layerProperties.layerName == layerName) {
      this->enabledLayerNameList.push_back(layerName);
      foundLayer = true;
    }
  }

  return foundLayer;
}

std::vector<VkExtensionProperties> Instance::getAvailableExtensions(
    std::string layerName) {

  if (layerName == "") {
    return this->extensionPropertyList;
  }

  uint32_t extensionPropertyCount = 0;
  VkResult result = vkEnumerateInstanceExtensionProperties(layerName.c_str(),
      &extensionPropertyCount, NULL);
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceExtensionProperties");
    exit(1);
  }

  std::vector<VkExtensionProperties> extensionPropertyList(
      extensionPropertyCount);
  result = vkEnumerateInstanceExtensionProperties(layerName.c_str(),
      &extensionPropertyCount, extensionPropertyList.data());
  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkEnumerateInstanceExtensionProperties");
    exit(1);
  }

  return extensionPropertyList;
}

bool Instance::addExtension(std::string extensionName, std::string layerName) {
  bool foundExtension = false;

  if (layerName == "") {
    for (VkExtensionProperties extensionProperties :
        this->extensionPropertyList) {
      if (extensionProperties.extensionName == extensionName) {
        this->enabledLayerNameList.push_back(extensionName);
        foundExtension = true;
      }
    }
  }
  else {
    uint32_t extensionPropertyCount = 0;
    VkResult result = vkEnumerateInstanceExtensionProperties(layerName.c_str(),
        &extensionPropertyCount, NULL);
    if (result != VK_SUCCESS) {
      PRINT_RETURN_CODE(stderr, result,
          "vkEnumerateInstanceExtensionProperties");
      exit(1);
    }

    std::vector<VkExtensionProperties> extensionPropertyList(
        extensionPropertyCount);
    result = vkEnumerateInstanceExtensionProperties(layerName.c_str(),
        &extensionPropertyCount, extensionPropertyList.data());
    if (result != VK_SUCCESS) {
      PRINT_RETURN_CODE(stderr, result,
          "vkEnumerateInstanceExtensionProperties");
      exit(1);
    }

    for (VkExtensionProperties extensionProperties :
        extensionPropertyList) {
      if (extensionProperties.extensionName == extensionName) {
        this->enabledLayerNameList.push_back(extensionName);
        foundExtension = true;
      }
    }
  }

  return foundExtension;
}

void Instance::activate() {
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
      vkCreateInstance(&instanceCreateInfo, NULL, &this->instance);

  free(enabledExtensionNamesUnsafe);
  free(enabledLayerNamesUnsafe);

  if (result != VK_SUCCESS) {
    PRINT_RETURN_CODE(stderr, result, "vkCreateInstance");
    exit(1);
  }

  this->isActive = true;
}
