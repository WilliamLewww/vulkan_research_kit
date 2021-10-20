#include "vrk/framebuffer.h"

Framebuffer::Framebuffer(VkDevice &deviceHandleRef,
                         VkRenderPass &renderPassHandleRef,
                         std::vector<VkImageView> imageViewHandleList,
                         VkFramebufferCreateFlags framebufferCreateFlags,
                         uint32_t width, uint32_t height, uint32_t layers)
    : deviceHandleRef(deviceHandleRef) {

  this->framebufferHandle = VK_NULL_HANDLE;

  VkFramebufferCreateInfo framebufferCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = NULL,
      .flags = framebufferCreateFlags,
      .renderPass = renderPassHandleRef,
      .attachmentCount = (uint32_t)imageViewHandleList.size(),
      .pAttachments = imageViewHandleList.data(),
      .width = width,
      .height = height,
      .layers = layers};

  VkResult result = vkCreateFramebuffer(deviceHandleRef, &framebufferCreateInfo,
                                        NULL, &this->framebufferHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateFramebuffer");
  }
}

Framebuffer::~Framebuffer() {
  vkDestroyFramebuffer(this->deviceHandleRef, this->framebufferHandle, NULL);
}

VkFramebuffer &Framebuffer::getFramebufferHandleRef() {
  return this->framebufferHandle;
}