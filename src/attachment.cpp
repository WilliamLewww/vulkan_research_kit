#include "vrk/attachment.h"

Attachment::Attachment(VkAttachmentDescriptionFlags attachmentDescriptionFlags,
    VkFormat format, VkSampleCountFlagBits sampleCountFlagBits,
    VkAttachmentLoadOp attachmentLoadOp,
    VkAttachmentStoreOp attachmentStoreOp,
    VkAttachmentLoadOp stencilAttachmentLoadOp,
    VkAttachmentStoreOp stencilAttachmentStoreOp,
    VkImageLayout initialImageLayout, VkImageLayout finalImageLayout) {

  this->attachmentDescription = {
    .flags = attachmentDescriptionFlags,
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