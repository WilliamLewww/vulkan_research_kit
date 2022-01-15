#include "vrk/sampler.h"

Sampler::Sampler(VkDevice &deviceHandleRef,
                 VkSamplerCreateFlags samplerCreateFlags, VkFilter magFilter,
                 VkFilter minFilter, VkSamplerMipmapMode samplerMipmapMode,
                 VkSamplerAddressMode samplerAddressModeU,
                 VkSamplerAddressMode samplerAddressModeV,
                 VkSamplerAddressMode samplerAddressModeW, float mipLodBias,
                 VkBool32 anisotropyEnable, float maxAnisotropy,
                 VkBool32 compareEnable, VkCompareOp compareOp, float minLod,
                 float maxLod, VkBorderColor borderColor,
                 VkBool32 unnormalizedCoordinates)
    : deviceHandleRef(deviceHandleRef) {

  VkSamplerCreateInfo samplerCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .pNext = NULL,
      .flags = samplerCreateFlags,
      .magFilter = magFilter,
      .minFilter = minFilter,
      .mipmapMode = samplerMipmapMode,
      .addressModeU = samplerAddressModeU,
      .addressModeV = samplerAddressModeV,
      .addressModeW = samplerAddressModeW,
      .mipLodBias = mipLodBias,
      .anisotropyEnable = anisotropyEnable,
      .maxAnisotropy = maxAnisotropy,
      .compareEnable = compareEnable,
      .compareOp = compareOp,
      .minLod = minLod,
      .maxLod = maxLod,
      .borderColor = borderColor,
      .unnormalizedCoordinates = unnormalizedCoordinates};

  VkResult result = vkCreateSampler(deviceHandleRef, &samplerCreateInfo, NULL,
                                    &this->samplerHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateSampler");
  }
}

Sampler::~Sampler() {
  vkDestroySampler(this->deviceHandleRef, this->samplerHandle, NULL);
}

VkSampler &Sampler::getSamplerHandleRef() { return this->samplerHandle; }
