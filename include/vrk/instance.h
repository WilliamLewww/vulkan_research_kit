#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Instance {
private:
  bool isActive;

  uint32_t majorVersion;
  uint32_t minorVersion;
  uint32_t patchVersion;

  VkInstance instance;

  std::vector<VkValidationFeatureEnableEXT> enabledValidationFeatureList;
  std::vector<VkValidationFeatureDisableEXT> disabledValidationFeatureList;

  VkValidationFeaturesEXT validationFeatures;
  VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
  VkApplicationInfo applicationInfo;

  uint32_t layerPropertyCount;
  std::vector<VkLayerProperties> layerPropertyList;
  std::vector<std::string> enabledLayerNameList;

  uint32_t extensionPropertyCount;
  std::vector<VkExtensionProperties> extensionPropertyList;
  std::vector<std::string> enabledExtensionNameList;
public:
  Instance();
  ~Instance();

  void addValidationFeatureEnable(
      VkValidationFeatureEnableEXT validationFeatureEnable);

  void addValidationFeatureDisable(
      VkValidationFeatureDisableEXT validationFeatureDisable);

  void setDebugUtilsMessageSeverityFlagBits(
      VkDebugUtilsMessageSeverityFlagBitsEXT
      debugUtilsMessageSeverityFlagBits);

  void setDebugUtilsMessageTypeFlagBits(VkDebugUtilsMessageTypeFlagBitsEXT
      debugUtilsMessageTypeFlagBitsEXT);

  std::vector<VkLayerProperties> getAvailableLayers();

  bool addLayer(std::string layerName);

  std::vector<VkExtensionProperties> getAvailableExtensions(
      std::string layerName = "");

  bool addExtension(std::string extensionName, std::string layerName = "");

  void activate();
};
