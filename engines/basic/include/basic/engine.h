#pragma once
#include "basic/material.h"

#include <vrk/command_buffer_group.h>
#include <vrk/command_pool.h>
#include <vrk/device.h>
#include <vrk/framebuffer.h>
#include <vrk/image.h>
#include <vrk/image_view.h>
#include <vrk/instance.h>
#include <vrk/render_pass.h>

#include <vrk/wsi/surface.h>
#include <vrk/wsi/swapchain.h>

#include <X11/Xlib.h>

class Engine {
public:
  Engine(std::string appName, bool enableValidation,
         std::vector<std::string> instanceLayerList,
         std::vector<std::string> instanceExtensionNameList);

  ~Engine();

  std::vector<std::string> getPhysicalDeviceNameList();

  void selectWindow(Display *displayPtr, std::shared_ptr<Window> windowPtr);

  void selectPhysicalDevice(std::string physicalDeviceName,
                            std::vector<std::string> deviceExtensionNameList);

private:
  std::unique_ptr<Instance> instancePtr;

  std::vector<VkPhysicalDevice> physicalDeviceHandleList;

  std::unique_ptr<Surface> surfacePtr;

  std::unique_ptr<VkPhysicalDevice> physicalDeviceHandlePtr;

  std::unique_ptr<Device> devicePtr;

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList;

  std::unique_ptr<CommandPool> commandPoolPtr;

  std::unique_ptr<CommandBufferGroup> commandBufferGroupPtr;

  VkSurfaceCapabilitiesKHR surfaceCapabilities;

  std::vector<VkSurfaceFormatKHR> surfaceFormatList;

  std::vector<VkPresentModeKHR> presentModeList;

  std::unique_ptr<Swapchain> swapchainPtr;

  std::unique_ptr<RenderPass> renderPassPtr;

  std::vector<VkImage> swapchainImageHandleList;

  std::vector<ImageView *> swapchainImageViewList;

  std::vector<Framebuffer *> framebufferList;
};
