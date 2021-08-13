#include "vrk/render_pass.h"

RenderPass::RenderPass(VkDevice& deviceHandleRef,
    VkRenderPassCreateFlagBits renderPassCreateFlagBits,
    std::vector<VkAttachmentDescription> attachmentDescriptionList,
    std::vector<VkSubpassDescription> subpassDescriptionList,
    std::vector<VkSubpassDependency> subpassDependencyList) :
    deviceHandleRef(deviceHandleRef) {

  this->renderPassHandle = VK_NULL_HANDLE;

  VkRenderPassCreateInfo renderPassCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    .pNext = NULL,
    .flags = renderPassCreateFlagBits,
    .attachmentCount = (uint32_t)attachmentDescriptionList.size(),
    .pAttachments = attachmentDescriptionList.data(),
    .subpassCount = (uint32_t)subpassDescriptionList.size(),
    .pSubpasses = subpassDescriptionList.data(),
    .dependencyCount = (uint32_t)subpassDependencyList.size(),
    .pDependencies = subpassDependencyList.data()
  };

  VkResult result = vkCreateRenderPass(deviceHandleRef,
      &renderPassCreateInfo, NULL, &this->renderPassHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateRenderPass");
  }
}

RenderPass::~RenderPass() {
  vkDestroyRenderPass(this->deviceHandleRef, this->renderPassHandle, NULL);
}