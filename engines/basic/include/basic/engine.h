#pragma once
#include "basic/material.h"
#include "basic/model.h"

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

  std::shared_ptr<Material> createMaterial(std::string materialName,
                                           std::string vertexFileName,
                                           std::string fragmentFileName);

  std::shared_ptr<Model> createModel(std::string modelName,
                                     std::string modelPath,
                                     std::shared_ptr<Material> materialPtr);

private:
  std::unique_ptr<Instance> instancePtr;

  std::vector<VkPhysicalDevice> physicalDeviceHandleList;

  std::unique_ptr<Surface> surfacePtr;

  std::unique_ptr<VkPhysicalDevice> physicalDeviceHandlePtr;

  std::shared_ptr<Device> devicePtr;

  std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList;

  std::unique_ptr<CommandPool> commandPoolPtr;

  std::unique_ptr<CommandBufferGroup> commandBufferGroupPtr;

  VkSurfaceCapabilitiesKHR surfaceCapabilities;

  std::vector<VkSurfaceFormatKHR> surfaceFormatList;

  std::vector<VkPresentModeKHR> presentModeList;

  std::unique_ptr<Swapchain> swapchainPtr;

  std::unique_ptr<RenderPass> renderPassPtr;

  std::vector<VkImage> swapchainImageHandleList;

  std::vector<std::unique_ptr<ImageView>> swapchainImageViewPtrList;

  std::vector<std::unique_ptr<Framebuffer>> framebufferPtrList;

  std::vector<std::shared_ptr<Material>> materialPtrList;

  std::vector<std::shared_ptr<Model>> modelPtrList;
};
