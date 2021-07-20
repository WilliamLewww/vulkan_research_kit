#include "vrk/engine.h"

Engine::Engine() {
  this->instance = new Instance();

  this->instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
  this->instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT);
  this->instance->addValidationFeatureEnable(
      VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);

  this->instance->setDebugUtilsMessageSeverityFlagBits(
      (VkDebugUtilsMessageSeverityFlagBitsEXT)
      (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT));

  this->instance->setDebugUtilsMessageTypeFlagBits(
    (VkDebugUtilsMessageTypeFlagBitsEXT)
    (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT));

  this->instance->getAvailableLayers();
  this->instance->addLayer("VK_LAYER_KHRONOS_validation");

  this->instance->activate();
}

Engine::~Engine() {
  delete this->instance;
}
