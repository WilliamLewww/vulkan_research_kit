#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

class Instance {
private:
  bool isActive;

  VkInstance instanceHandle;
  VkDebugUtilsMessengerEXT debugUtilsMessengerHandle;

  uint32_t majorVersion;
  uint32_t minorVersion;
  uint32_t patchVersion;

  VkValidationFeaturesEXT validationFeatures;
  VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
  VkApplicationInfo applicationInfo;

  std::vector<VkValidationFeatureEnableEXT> enabledValidationFeatureList;
  std::vector<VkValidationFeatureDisableEXT> disabledValidationFeatureList;

  std::vector<VkLayerProperties> layerPropertiesList;
  std::vector<VkExtensionProperties> extensionPropertiesList;

  std::vector<std::string> enabledLayerNameList;
  std::vector<std::string> enabledExtensionNameList;
public:
  Instance();
  ~Instance();

  std::string getVulkanVersionAPI();

  void addValidationFeatureEnable(
      VkValidationFeatureEnableEXT validationFeatureEnable);

  void addValidationFeatureDisable(
      VkValidationFeatureDisableEXT validationFeatureDisable);

  void setDebugUtilsMessageSeverityFlagBits(
      VkDebugUtilsMessageSeverityFlagBitsEXT debugUtilsMessageSeverityFlagBits);

  void setDebugUtilsMessageTypeFlagBits(
      VkDebugUtilsMessageTypeFlagBitsEXT debugUtilsMessageTypeFlagBitsEXT);

  std::vector<VkLayerProperties> getAvailableLayerPropertiesList();

  bool addLayer(std::string layerName);

  std::vector<VkExtensionProperties> getAvailableExtensionPropertiesList(
      std::string layerName = "");

  bool addExtension(std::string extensionName, std::string layerName = "");

  void activate();

  VkInstance getInstanceHandle();
};
