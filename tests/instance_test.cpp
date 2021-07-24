#include <gtest/gtest.h>

#include <vrk/instance.h>

TEST (InstanceTest, Default) {
  Instance* instance = new Instance();
  instance->activate();

  std::string version = instance->getVulkanVersionAPI();
  EXPECT_FALSE(version == "");

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, DoubleActivation) {
  Instance* instance = new Instance();
  instance->activate();

  std::stringstream buffer;
  std::streambuf *sbuf = std::cerr.rdbuf();
  std::cerr.rdbuf(buffer.rdbuf());

  instance->activate();
  EXPECT_STREQ("Instance is already active\n", buffer.str().c_str());

  std::cerr.rdbuf(sbuf);

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, AddAllLayers) {
  Instance* instance = new Instance();

  std::vector<VkLayerProperties> layerPropertyList =
      instance->getAvailableLayerPropertiesList();

  std::vector<std::string> ignoredLayerNameList = {
    "VK_LAYER_LUNARG_gfxreconstruct",
    "VK_LAYER_LUNARG_api_dump"
  };

  for (VkLayerProperties layerProperties : layerPropertyList) {
    if (std::find_if(std::begin(ignoredLayerNameList),
        std::end(ignoredLayerNameList),
        [&](const std::string& x) { return x == layerProperties.layerName; })
        == std::end(ignoredLayerNameList)) {

      int result = instance->addLayer(layerProperties.layerName);
      EXPECT_EQ(result, true);
    }
  }

  instance->activate();

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, AddAllExtensions) {
  Instance* instance = new Instance();

  std::vector<VkExtensionProperties> extensionPropertiesList =
      instance->getAvailableExtensionPropertiesList();

  for (VkExtensionProperties extensionProperties : extensionPropertiesList) {
    int result = instance->addExtension(extensionProperties.extensionName);
    EXPECT_EQ(result, true);
  }

  instance->activate();

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, AddAllExtensionsFromLayer) {
  Instance* instance = new Instance();

  instance->setDebugUtilsMessageSeverityFlagBits(
      (VkDebugUtilsMessageSeverityFlagBitsEXT)
      (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT));

  instance->setDebugUtilsMessageTypeFlagBits(
      (VkDebugUtilsMessageTypeFlagBitsEXT)
      (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT));

  int result = instance->addLayer("VK_LAYER_KHRONOS_validation");
  EXPECT_EQ(result, true);

  std::vector<VkExtensionProperties> extensionPropertiesList =
      instance->getAvailableExtensionPropertiesList(
      "VK_LAYER_KHRONOS_validation");

  for (VkExtensionProperties extensionProperties : extensionPropertiesList) {
    int result = instance->addExtension(extensionProperties.extensionName,
        "VK_LAYER_KHRONOS_validation");
    EXPECT_EQ(result, true);
  }

  std::stringstream buffer;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  instance->activate();

  std::cout.rdbuf(sbuf);

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, ValidationLayer) {
  Instance* instance = new Instance();

  instance->setDebugUtilsMessageSeverityFlagBits(
      (VkDebugUtilsMessageSeverityFlagBitsEXT)
      (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT));

  instance->setDebugUtilsMessageTypeFlagBits(
      (VkDebugUtilsMessageTypeFlagBitsEXT)
      (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT));

  instance->addLayer("VK_LAYER_KHRONOS_validation");
  instance->addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  std::stringstream buffer;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  instance->activate();
  std::string output = buffer.str();
  EXPECT_STREQ("Validation Information", output.substr(5, 22).c_str());

  std::cout.rdbuf(sbuf);

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, AddInvalidLayer) {
  Instance* instance = new Instance();

  bool result = instance->addLayer("NOT_A_REAL_LAYER");
  EXPECT_EQ(result, false);

  instance->activate();

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, GetExtensionsOnInvalidLayer) {
  Instance* instance = new Instance();

  std::vector<VkExtensionProperties> extensionPropertiesList =
      instance->getAvailableExtensionPropertiesList("NOT_A_REAL_LAYER");

  EXPECT_EQ(extensionPropertiesList.size(), 0);

  instance->activate();

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, AddInvalidExtension) {
  Instance* instance = new Instance();

  bool result = instance->addExtension("NOT_A_REAL_EXTENSION");
  EXPECT_EQ(result, false);

  instance->activate();

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, AddExtensionOnInvalidLayer) {
  Instance* instance = new Instance();

  bool result = instance->addExtension("NOT_A_REAL_EXTENSION",
      "NOT_A_REAL_LAYER");

  EXPECT_EQ(result, false);

  instance->activate();

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}