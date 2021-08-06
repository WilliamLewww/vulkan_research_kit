#include "vrk/attachment.h"

Attachment::Attachment(VkFormat format,
    VkSampleCountFlagBits sampleCountFlagBits,
    VkAttachmentLoadOp attachmentLoadOp,
    VkAttachmentStoreOp attachmentStoreOp,
    VkAttachmentLoadOp stencilAttachmentLoadOp,
    VkAttachmentStoreOp stencilAttachmentStoreOp,
    VkImageLayout initialImageLayout,
    VkImageLayout finalImageLayout) {

  this->attachmentDescription = {
    .flags = 0,
    .format = format,
    .samples = sampleCountFlagBits,
    .loadOp = attachmentLoadOp,
    .storeOp = attachmentStoreOp,
    .stencilLoadOp = stencilAttachmentLoadOp,
    .stencilStoreOp = stencilAttachmentStoreOp,
    .initialLayout = initialImageLayout,
    .finalLayout = finalImageLayout
  };
}

Attachment::~Attachment() {

}

VkAttachmentDescription Attachment::getAttachmentDescription() {
  return this->attachmentDescription;
}