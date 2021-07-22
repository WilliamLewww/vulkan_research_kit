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

  for (VkLayerProperties layerProperties : layerPropertyList) {
    instance->addLayer(layerProperties.layerName);
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
    instance->addExtension(extensionProperties.extensionName);
  }

  instance->activate();

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

TEST (InstanceTest, ValidationLayer) {
  Instance* instance = new Instance();

  std::vector<VkExtensionProperties> extensionPropertiesList =
      instance->getAvailableExtensionPropertiesList();

  for (VkExtensionProperties extensionProperties : extensionPropertiesList) {
    instance->addExtension(extensionProperties.extensionName);
  }

  instance->activate();

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  delete instance;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}