#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <algorithm>

class Instance {
public:
  static std::vector<VkLayerProperties> getAvailableLayerPropertiesList();

  static std::vector<VkExtensionProperties> getAvailableExtensionPropertiesList(
      std::string layerName = "");

  Instance(
      std::vector<VkValidationFeatureEnableEXT> validationFeatureEnableList,
      std::vector<VkValidationFeatureDisableEXT> validationFeatureDisableList,
      VkDebugUtilsMessageSeverityFlagBitsEXT debugUtilsMessageSeverityFlagBits,
      VkDebugUtilsMessageTypeFlagBitsEXT debugUtilsMessageTypeFlagBits,
      std::string applicationName,
      uint32_t applicationVersion,
      std::vector<std::string> enabledLayerNameList,
      std::vector<std::string> enabledExtensionNameList);

  ~Instance();

  std::string getVulkanVersionAPI();

  VkInstance& getInstanceHandleRef();
private:
  VkInstance instanceHandle;

  VkDebugUtilsMessengerEXT debugUtilsMessengerHandle;

  bool debugUtilsExtensionEnabled;

  uint32_t majorVersion;
  uint32_t minorVersion;
  uint32_t patchVersion;
};