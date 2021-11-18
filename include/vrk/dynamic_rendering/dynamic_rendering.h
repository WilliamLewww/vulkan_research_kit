#pragma once

#include "vrk/helper.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

const std::vector<std::string>
    DYNAMIC_RENDERING_REQUIRED_INSTANCE_EXTENSION_LIST = {
        "VK_KHR_get_physical_device_properties2"};

const std::vector<std::string>
    DYNAMIC_RENDERING_REQUIRED_DEVICE_EXTENSION_LIST = {
        "VK_KHR_dynamic_rendering"};

namespace DynamicRendering {
struct RenderingAttachmentInfoParam {
  VkImageView imageViewHandle;
  VkImageLayout imageLayout;
  VkResolveModeFlagBits resolveModeFlagBits;
  VkImageView resolveImageViewHandle;
  VkImageLayout resolveImageLayout;
  VkAttachmentLoadOp attachmentLoadOp;
  VkAttachmentStoreOp attachmentStoreOp;
  VkClearValue clearValue;
};

void beginRenderingCmd(VkDevice &deviceHandleRef,
                       VkCommandBuffer &commandBufferHandleRef,
                       VkRenderingFlagsKHR renderingFlags,
                       VkRect2D rect2DRenderArea, uint32_t layerCount,
                       uint32_t viewMask, uint32_t colorAttachmentCount,
                       std::vector<RenderingAttachmentInfoParam>
                           colorRenderingAttachmentInfoParamList,
                       std::shared_ptr<RenderingAttachmentInfoParam>
                           depthRenderingAttachmentInfoParamPtr,
                       std::shared_ptr<RenderingAttachmentInfoParam>
                           stencilRenderingAttachmentInfoParamPtr);

void endRenderingCmd(VkDevice &deviceHandleRef,
                     VkCommandBuffer &commandBufferHandleRef);
} // namespace DynamicRendering
