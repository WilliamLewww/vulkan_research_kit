#include "vrk/framebuffer.h"

Framebuffer::Framebuffer(VkDevice* deviceHandlePtr,
    VkRenderPass* renderPassHandlePtr, 
    VkImageView* initialAttachmentImageViewHandlePtr,
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

  this->attachmentImageViewHandleList = {};

  if (initialAttachmentImageViewHandlePtr != NULL) {
    this->attachmentImageViewHandleList.push_back(
        *initialAttachmentImageViewHandlePtr);
  }
}

Framebuffer::~Framebuffer() {
  vkDestroyFramebuffer(*this->deviceHandlePtr, this->framebufferHandle, NULL);
}

void Framebuffer::addAttachmentImageViewHandle(
    VkImageView* attachmentImageViewHandlePtr) {

  this->attachmentImageViewHandleList.push_back(*attachmentImageViewHandlePtr);
}

bool Framebuffer::activate() {
  if (!Component::activate()) {
    return false;
  }

  this->framebufferCreateInfo.attachmentCount =
      (uint32_t)this->attachmentImageViewHandleList.size();
  this->framebufferCreateInfo.pAttachments =
      this->attachmentImageViewHandleList.data();

  VkResult result = vkCreateFramebuffer(*this->deviceHandlePtr,
      &framebufferCreateInfo, NULL, &this->framebufferHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateFramebuffer");
  }

  return true;
}