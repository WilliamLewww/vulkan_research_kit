#include "vrk/framebuffer.h"

Framebuffer::Framebuffer(VkDevice* deviceHandlePtr,
    VkRenderPass* renderPassHandlePtr, 
    std::vector<VkImageView>* attachmentImageViewHandleListPtr,
    VkFramebufferCreateFlags framebufferCreateFlags,
    uint32_t width, uint32_t height, uint32_t layers) :
    Component("framebuffer") {

  this->framebufferHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;

  this->framebufferCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    .pNext = NULL,
    .flags = framebufferCreateFlags,
    .renderPass = *renderPassHandlePtr,
    .attachmentCount = 0,
    .pAttachments = NULL,
    .width = width,
    .height = height,
    .layers = layers
  };

  if (attachmentImageViewHandleListPtr != NULL) {
    this->framebufferCreateInfo.attachmentCount =
        (uint32_t)attachmentImageViewHandleListPtr->size();
    this->framebufferCreateInfo.pAttachments =
        attachmentImageViewHandleListPtr->data();
  }
}

Framebuffer::~Framebuffer() {
  vkDestroyFramebuffer(*this->deviceHandlePtr, this->framebufferHandle, NULL);
}

bool Framebuffer::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkResult result = vkCreateFramebuffer(*this->deviceHandlePtr,
      &framebufferCreateInfo, NULL, &this->framebufferHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateFramebuffer");
  }

  return true;
}