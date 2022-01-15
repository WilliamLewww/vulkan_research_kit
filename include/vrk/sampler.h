#pragma once

#include "vrk/device.h"
#include "vrk/helper.h"

#include <vulkan/vulkan.h>

class Sampler {
public:
  Sampler(VkDevice &deviceHandleRef, VkSamplerCreateFlags samplerCreateFlags,
          VkFilter magFilter, VkFilter minFilter,
          VkSamplerMipmapMode samplerMipmapMode,
          VkSamplerAddressMode samplerAddressModeU,
          VkSamplerAddressMode samplerAddressModeV,
          VkSamplerAddressMode samplerAddressModeW, float mipLodBias,
          VkBool32 anisotropyEnable, float maxAnisotropy,
          VkBool32 compareEnable, VkCompareOp compareOp, float minLod,
          float maxLod, VkBorderColor borderColor,
          VkBool32 unnormalizedCoordinates);

  ~Sampler();

  VkSampler &getSamplerHandleRef();

private:
  VkSampler samplerHandle;

  VkDevice &deviceHandleRef;
};
