#pragma once

#include "vrk/helper.h"
#include "vrk/component.h"
#include "vrk/attachment.h"

#include <vulkan/vulkan.h>

#include <vector>

class RenderPass : public Component {
private:
  VkRenderPass renderPassHandle;

  VkDevice* deviceHandlePtr;

  std::vector<VkAttachmentDescription> attachmentDescriptionList;
  std::vector<VkSubpassDescription> subpassDescriptionList;
  std::vector<VkSubpassDependency> subpassDependencyList;

  std::vector<std::vector<VkAttachmentReference>>
      subpassColorAttachmentReferenceList;

  std::vector<VkAttachmentReference> subpassDepthAttachmentReferenceList;
public:
  RenderPass(VkDevice* deviceHandlePtr,
      VkPipelineBindPoint initialSubpassPipelineBindPoint,
      std::vector<Attachment> initialColorAttachmentList,
      std::vector<VkImageLayout> initialColorImageLayoutList,
      Attachment* initialDepthAttachmentPtr = NULL,
      VkImageLayout* initialDepthImageLayoutPtr = NULL);

  ~RenderPass();

  uint32_t addAttachment(Attachment attachment);

  uint32_t addSubpass(VkPipelineBindPoint pipelineBindPoint,
      std::vector<uint32_t> colorAttachmentIndexList,
      std::vector<VkImageLayout> colorImageLayoutList,
      uint32_t* depthAttachmentIndexPtr = NULL,
      VkImageLayout* depthImageLayoutPtr = NULL);

  void addDependency(uint32_t srcSubpass, uint32_t dstSubpass,
      VkPipelineStageFlags srcPipelineStageMask,
      VkPipelineStageFlags dstPipelineStageMask,
      VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
      VkDependencyFlags dependencyFlags);

  bool activate();

  VkRenderPass* getRenderPassHandlePtr();
};