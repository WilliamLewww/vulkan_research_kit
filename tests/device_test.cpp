#include <gtest/gtest.h>

#include <vrk/instance.h>
#include <vrk/device.h>

TEST (DeviceTest, Default) {
  Instance* instance = new Instance();
  instance->activate();

  std::string version = instance->getVulkanVersionAPI();
  EXPECT_FALSE(version == "");

  EXPECT_NE(instance->getInstanceHandle(), VK_NULL_HANDLE);

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandle());

  VkPhysicalDevice activePhysicalDevice;
  for (VkPhysicalDevice deviceHandle : deviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties = 
        Device::getPhysicalDeviceProperties(deviceHandle);

    if (physicalDeviceProperties.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      activePhysicalDevice = deviceHandle;
      break;
    }
  }

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList = 
      Device::getQueueFamilyPropertiesList(activePhysicalDevice);

  uint32_t queueFamilyIndex = -1;
  for (uint32_t x = 0; x < queueFamilyPropertiesList.size(); x++) {
    if (queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndex = x;
    }
  }

  Device* device = new Device(instance->getInstanceHandle(), 
      activePhysicalDevice, queueFamilyIndex, 1);

  device->activate();

  EXPECT_NE(device->getDeviceHandle(), VK_NULL_HANDLE);

  delete device;
  delete instance;
}

TEST (DeviceTest, InvalidInstanceHandle) {
  Device* device = nullptr;

  EXPECT_EQ(device, nullptr);

  delete device;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}