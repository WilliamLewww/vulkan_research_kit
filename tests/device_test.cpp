#include <gtest/gtest.h>

#include <vrk/instance.h>
#include <vrk/device.h>

TEST (DeviceTest, GetPhysicalDevices) {
  Instance* instance = new Instance();
  instance->activate();
  EXPECT_NE(*instance->getInstanceHandlePtr(), VK_NULL_HANDLE);

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandlePtr());

  EXPECT_GT(deviceHandleList.size(), 0);

  delete instance;
}

TEST (DeviceTest, GetPhysicalDeviceProperties) {
  Instance* instance = new Instance();
  instance->activate();
  EXPECT_NE(*instance->getInstanceHandlePtr(), VK_NULL_HANDLE);

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandlePtr());

  for (VkPhysicalDevice deviceHandle : deviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties = 
        Device::getPhysicalDeviceProperties(&deviceHandle);
  }

 delete instance;
}

TEST (DeviceTest, GetQueueFamilyPropertiesList) {
  Instance* instance = new Instance();
  instance->activate();
  EXPECT_NE(*instance->getInstanceHandlePtr(), VK_NULL_HANDLE);

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandlePtr());

  for (VkPhysicalDevice deviceHandle : deviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties = 
        Device::getPhysicalDeviceProperties(&deviceHandle);

    std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList = 
        Device::getQueueFamilyPropertiesList(&deviceHandle);

    EXPECT_GT(queueFamilyPropertiesList.size(), 0);
  }

 delete instance;
}

TEST (DeviceTest, Default) {
  Instance* instance = new Instance();
  instance->activate();
  EXPECT_NE(*instance->getInstanceHandlePtr(), VK_NULL_HANDLE);

  std::vector<VkPhysicalDevice> deviceHandleList =
      Device::getPhysicalDevices(instance->getInstanceHandlePtr());

  VkPhysicalDevice activePhysicalDevice;
  for (VkPhysicalDevice deviceHandle : deviceHandleList) {
    VkPhysicalDeviceProperties physicalDeviceProperties = 
        Device::getPhysicalDeviceProperties(&deviceHandle);

    if (physicalDeviceProperties.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      activePhysicalDevice = deviceHandle;
      break;
    }
  }

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList = 
      Device::getQueueFamilyPropertiesList(&activePhysicalDevice);

  uint32_t queueFamilyIndex = -1;
  for (uint32_t x = 0; x < queueFamilyPropertiesList.size(); x++) {
    if (queueFamilyPropertiesList[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndex = x;
    }
  }

  Device* device = new Device(instance->getInstanceHandlePtr(), 
      &activePhysicalDevice, queueFamilyIndex, 1);

  device->activate();

  EXPECT_NE(*device->getDeviceHandlePtr(), VK_NULL_HANDLE);

  delete device;
  delete instance;
}

TEST (DeviceTest, InvalidInstanceHandle) {
  VkInstance instanceHandle = VK_NULL_HANDLE;
  VkPhysicalDevice activePhysicalDevice = VK_NULL_HANDLE;

  Device* device = nullptr;
  try {
    device = new Device(&instanceHandle, 
        &activePhysicalDevice, 0, 1);
  }
  catch(std::exception& e) {
    std::stringstream buffer;
    std::streambuf *sbuf = std::cerr.rdbuf();
    std::cerr.rdbuf(buffer.rdbuf());

    std::cerr << e.what() << std::endl;
    EXPECT_STREQ("Invalid instance handle\n", buffer.str().c_str());

    std::cerr.rdbuf(sbuf);
  }

  EXPECT_EQ(device, nullptr);

  delete device;
}

TEST (DeviceTest, InvalidPhysicalDeviceHandle) {
  Instance* instance = new Instance();
  instance->activate();

  VkPhysicalDevice activePhysicalDevice = VK_NULL_HANDLE;

  Device* device = nullptr;

  try {
    device = new Device(instance->getInstanceHandlePtr(), &activePhysicalDevice, 0, 1);
  }
  catch(std::exception& e) {
    std::stringstream buffer;
    std::streambuf *sbuf = std::cerr.rdbuf();
    std::cerr.rdbuf(buffer.rdbuf());

    std::cerr << e.what() << std::endl;
    EXPECT_STREQ("Invalid physical device handle\n", buffer.str().c_str());

    std::cerr.rdbuf(sbuf);
  }

  EXPECT_EQ(device, nullptr);

  delete device;
  delete instance;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}