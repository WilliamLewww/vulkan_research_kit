#include "vrk/dynamic_rendering/dynamic_rendering.h"

void DynamicRendering::beginRenderingCmd(
    VkDevice &deviceHandleRef, VkCommandBuffer &commandBufferHandleRef,
    VkRenderingFlagsKHR renderingFlags, VkRect2D rect2DRenderArea,
    uint32_t layerCount, uint32_t viewMask, uint32_t colorAttachmentCount,
    std::vector<RenderingAttachmentInfoParam>
        colorRenderingAttachmentInfoParamList,
    std::shared_ptr<RenderingAttachmentInfoParam>
        depthRenderingAttachmentInfoParamPtr,
    std::shared_ptr<RenderingAttachmentInfoParam>
        stencilRenderingAttachmentInfoParamPtr) {

  std::vector<VkRenderingAttachmentInfoKHR> colorRenderingAttachmentInfoList;

  for (RenderingAttachmentInfoParam renderingAttachmentInfoParam :
       colorRenderingAttachmentInfoParamList) {

    VkRenderingAttachmentInfoKHR renderingAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .pNext = NULL,
        .imageView = renderingAttachmentInfoParam.imageViewHandle,
        .imageLayout = renderingAttachmentInfoParam.imageLayout,
        .resolveMode = renderingAttachmentInfoParam.resolveModeFlagBits,
        .resolveImageView = renderingAttachmentInfoParam.resolveImageViewHandle,
        .resolveImageLayout = renderingAttachmentInfoParam.resolveImageLayout,
        .loadOp = renderingAttachmentInfoParam.attachmentLoadOp,
        .storeOp = renderingAttachmentInfoParam.attachmentStoreOp,
        .clearValue = renderingAttachmentInfoParam.clearValue};

    colorRenderingAttachmentInfoList.push_back(renderingAttachmentInfo);
  }

  VkRenderingAttachmentInfoKHR depthRenderingAttachmentInfo;
  VkRenderingAttachmentInfoKHR *depthRenderingAttachmentInfoPtr = NULL;
  if (depthRenderingAttachmentInfoParamPtr) {
    depthRenderingAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .pNext = NULL,
        .imageView = depthRenderingAttachmentInfoParamPtr->imageViewHandle,
        .imageLayout = depthRenderingAttachmentInfoParamPtr->imageLayout,
        .resolveMode =
            depthRenderingAttachmentInfoParamPtr->resolveModeFlagBits,
        .resolveImageView =
            depthRenderingAttachmentInfoParamPtr->resolveImageViewHandle,
        .resolveImageLayout =
            depthRenderingAttachmentInfoParamPtr->resolveImageLayout,
        .loadOp = depthRenderingAttachmentInfoParamPtr->attachmentLoadOp,
        .storeOp = depthRenderingAttachmentInfoParamPtr->attachmentStoreOp,
        .clearValue = depthRenderingAttachmentInfoParamPtr->clearValue};

    depthRenderingAttachmentInfoPtr = &depthRenderingAttachmentInfo;
  }

  VkRenderingAttachmentInfoKHR stencilRenderingAttachmentInfo;
  VkRenderingAttachmentInfoKHR *stencilRenderingAttachmentInfoPtr = NULL;
  if (stencilRenderingAttachmentInfoParamPtr) {
    stencilRenderingAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .pNext = NULL,
        .imageView = stencilRenderingAttachmentInfoParamPtr->imageViewHandle,
        .imageLayout = stencilRenderingAttachmentInfoParamPtr->imageLayout,
        .resolveMode =
            stencilRenderingAttachmentInfoParamPtr->resolveModeFlagBits,
        .resolveImageView =
            stencilRenderingAttachmentInfoParamPtr->resolveImageViewHandle,
        .resolveImageLayout =
            stencilRenderingAttachmentInfoParamPtr->resolveImageLayout,
        .loadOp = stencilRenderingAttachmentInfoParamPtr->attachmentLoadOp,
        .storeOp = stencilRenderingAttachmentInfoParamPtr->attachmentStoreOp,
        .clearValue = stencilRenderingAttachmentInfoParamPtr->clearValue};

    stencilRenderingAttachmentInfoPtr = &stencilRenderingAttachmentInfo;
  }

  VkRenderingInfoKHR renderingInfo = {
      .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
      .pNext = NULL,
      .flags = renderingFlags,
      .renderArea = rect2DRenderArea,
      .layerCount = layerCount,
      .viewMask = viewMask,
      .colorAttachmentCount = (uint32_t)colorRenderingAttachmentInfoList.size(),
      .pColorAttachments = colorRenderingAttachmentInfoList.data(),
      .pDepthAttachment = depthRenderingAttachmentInfoPtr,
      .pStencilAttachment = stencilRenderingAttachmentInfoPtr};

  LOAD_DEVICE_FUNCTION(deviceHandleRef, vkCmdBeginRenderingKHR,
                       pvkCmdBeginRenderingKHR);

  pvkCmdBeginRenderingKHR(commandBufferHandleRef, &renderingInfo);
}

void DynamicRendering::endRenderingCmd(
    VkDevice &deviceHandleRef, VkCommandBuffer &commandBufferHandleRef) {

  LOAD_DEVICE_FUNCTION(deviceHandleRef, vkCmdEndRenderingKHR,
                       pvkCmdEndRenderingKHR);

  pvkCmdEndRenderingKHR(commandBufferHandleRef);
}
