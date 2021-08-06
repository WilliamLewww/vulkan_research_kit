#include "vrk/render_pass.h"

RenderPass::RenderPass(VkDevice* deviceHandlePtr,
    VkPipelineBindPoint initialSubpassPipelineBindPoint,
    std::vector<Attachment> initialColorAttachmentList,
    std::vector<VkImageLayout> initialColorImageLayoutList,
    Attachment* initialDepthAttachmentPtr,
    VkImageLayout* initialDepthImageLayoutPtr) :
    Component("render pass") {

  this->renderPassHandle = VK_NULL_HANDLE;

  this->deviceHandlePtr = deviceHandlePtr;

  this->attachmentDescriptionList = {};

  std::vector<VkAttachmentReference> colorAttachmentReferenceList = {};

  for (uint32_t x = 0; x < initialColorAttachmentList.size(); x++) {
    this->attachmentDescriptionList.push_back(
        initialColorAttachmentList[x].getAttachmentDescription());

    VkAttachmentReference colorAttachmentReference = {
      .attachment = x,
      .layout = initialColorImageLayoutList[x]
    };
    colorAttachmentReferenceList.push_back(colorAttachmentReference);
  }

  this->subpassColorAttachmentReferenceList.push_back(
      colorAttachmentReferenceList);

  VkAttachmentReference* depthAttachmentReferencePtr = NULL;
  if (initialDepthAttachmentPtr != NULL) {
    this->attachmentDescriptionList.push_back(
        initialDepthAttachmentPtr->getAttachmentDescription());

    VkAttachmentReference depthAttachmentReference = {
      .attachment = (uint32_t)initialColorAttachmentList.size(),
      .layout = *initialDepthImageLayoutPtr
    };
    this->subpassDepthAttachmentReferenceList.push_back(
        depthAttachmentReference);

    depthAttachmentReferencePtr = &this->subpassDepthAttachmentReferenceList[0];
  }

  VkSubpassDescription subpassDescription = {
    .flags = 0,
    .pipelineBindPoint = initialSubpassPipelineBindPoint,
    .inputAttachmentCount = 0,
    .pInputAttachments = NULL,
    .colorAttachmentCount =
        (uint32_t)this->subpassColorAttachmentReferenceList[0].size(),
    .pColorAttachments = this->subpassColorAttachmentReferenceList[0].data(),
    .pResolveAttachments = NULL,
    .pDepthStencilAttachment = depthAttachmentReferencePtr,
    .preserveAttachmentCount = 0,
    .pPreserveAttachments = NULL,
  };

  this->subpassDescriptionList = {};
  this->subpassDescriptionList.push_back(subpassDescription);

  this->subpassDependencyList = {};
}

RenderPass::~RenderPass() {
  vkDestroyRenderPass(*this->deviceHandlePtr, this->renderPassHandle, NULL);
}

uint32_t RenderPass::addAttachment(Attachment attachment) {
  this->attachmentDescriptionList.push_back(
      attachment.getAttachmentDescription());

  return this->attachmentDescriptionList.size() - 1;
}

uint32_t RenderPass::addSubpass(VkPipelineBindPoint pipelineBindPoint,
    std::vector<uint32_t> colorAttachmentIndexList,
    std::vector<VkImageLayout> colorImageLayoutList,
    uint32_t* depthAttachmentIndexPtr,
    VkImageLayout* depthImageLayoutPtr) {

  std::vector<VkAttachmentReference> colorAttachmentReferenceList = {};

  for (uint32_t x = 0; x < colorAttachmentIndexList.size(); x++) {
    VkAttachmentReference colorAttachmentReference = {
      .attachment = colorAttachmentIndexList[x],
      .layout = colorImageLayoutList[x]
    };
    colorAttachmentReferenceList.push_back(colorAttachmentReference);
  }

  this->subpassColorAttachmentReferenceList.push_back(
      colorAttachmentReferenceList);

  std::vector<VkAttachmentReference>* subpassColorAttachmentReferenceListPtr =
      &this->subpassColorAttachmentReferenceList[
      this->subpassColorAttachmentReferenceList.size() - 1];

  VkAttachmentReference* depthAttachmentReferencePtr = NULL;
  if (depthAttachmentIndexPtr != NULL) {
    VkAttachmentReference depthAttachmentReference = {
      .attachment = *depthAttachmentIndexPtr,
      .layout = *depthImageLayoutPtr
    };
    this->subpassDepthAttachmentReferenceList.push_back(
        depthAttachmentReference);

    depthAttachmentReferencePtr = &this->subpassDepthAttachmentReferenceList[
        this->subpassDepthAttachmentReferenceList.size() - 1];
  }

  VkSubpassDescription subpassDescription = {
    .flags = 0,
    .pipelineBindPoint = pipelineBindPoint,
    .inputAttachmentCount = 0,
    .pInputAttachments = NULL,
    .colorAttachmentCount =
        (uint32_t)subpassColorAttachmentReferenceListPtr->size(),
    .pColorAttachments = subpassColorAttachmentReferenceListPtr->data(),
    .pResolveAttachments = NULL,
    .pDepthStencilAttachment = depthAttachmentReferencePtr,
    .preserveAttachmentCount = 0,
    .pPreserveAttachments = NULL,
  };

  this->subpassDescriptionList.push_back(subpassDescription);

  return this->subpassDescriptionList.size() - 1;
}

void RenderPass::addDependency(uint32_t srcSubpass,
    uint32_t dstSubpass,
    VkPipelineStageFlags srcPipelineStageMask,
    VkPipelineStageFlags dstPipelineStageMask,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask,
    VkDependencyFlags dependencyFlags) {

  VkSubpassDependency subpassDependency = {
    .srcSubpass = srcSubpass,
    .dstSubpass = dstSubpass,
    .srcStageMask = srcPipelineStageMask,
    .dstStageMask = dstPipelineStageMask,
    .srcAccessMask = srcAccessMask,
    .dstAccessMask = dstAccessMask,
    .dependencyFlags = dependencyFlags,
  };

  this->subpassDependencyList.push_back(subpassDependency);
}

bool RenderPass::activate() {
  if (!Component::activate()) {
    return false;
  }

  VkRenderPassCreateInfo renderPassCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .attachmentCount = (uint32_t)this->attachmentDescriptionList.size(),
    .pAttachments = this->attachmentDescriptionList.data(),
    .subpassCount = (uint32_t)this->subpassDescriptionList.size(),
    .pSubpasses = this->subpassDescriptionList.data(),
    .dependencyCount = (uint32_t)this->subpassDependencyList.size(),
    .pDependencies = this->subpassDependencyList.data()
  };

  VkResult result = vkCreateRenderPass(*this->deviceHandlePtr,
      &renderPassCreateInfo, NULL, &this->renderPassHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateRenderPass");
  }

  return true;
}

VkRenderPass* RenderPass::getRenderPassHandlePtr() {
  return &this->renderPassHandle;
}