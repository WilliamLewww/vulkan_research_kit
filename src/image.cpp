#include "vrk/image.h"

Image::Image(VkDevice &deviceHandleRef,
             VkPhysicalDevice &physicalDeviceHandleRef,
             VkImageCreateFlags imageCreateFlags, VkImageType imageType,
             VkFormat format, VkExtent3D extent3D, uint32_t mipLevels,
             uint32_t arrayLayers, VkSampleCountFlagBits sampleCountFlagBits,
             VkImageTiling imageTiling, VkImageUsageFlags imageUsageFlags,
             VkSharingMode sharingMode,
             std::vector<uint32_t> queueFamilyIndexList,
             VkImageLayout initialImageLayout,
             VkMemoryPropertyFlags memoryPropertyFlags)
    : deviceHandleRef(deviceHandleRef) {

  VkImageCreateInfo imageCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext = NULL,
      .flags = imageCreateFlags,
      .imageType = imageType,
      .format = format,
      .extent = extent3D,
      .mipLevels = mipLevels,
      .arrayLayers = arrayLayers,
      .samples = sampleCountFlagBits,
      .tiling = imageTiling,
      .usage = imageUsageFlags,
      .sharingMode = sharingMode,
      .queueFamilyIndexCount = (uint32_t)queueFamilyIndexList.size(),
      .pQueueFamilyIndices = queueFamilyIndexList.data(),
      .initialLayout = initialImageLayout};

  VkResult result = vkCreateImage(deviceHandleRef, &imageCreateInfo, NULL,
                                  &this->imageHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateImage");
  }

  VkMemoryRequirements memoryRequirements;
  vkGetImageMemoryRequirements(deviceHandleRef, this->imageHandle,
                               &memoryRequirements);

  VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties =
      Device::getPhysicalDeviceMemoryProperties(physicalDeviceHandleRef);

  uint32_t memoryTypeIndex = -1;
  for (uint32_t x = 0; x < physicalDeviceMemoryProperties.memoryTypeCount;
       x++) {
    if ((memoryRequirements.memoryTypeBits & (1 << x)) &&
        (physicalDeviceMemoryProperties.memoryTypes[x].propertyFlags &
         memoryPropertyFlags) == memoryPropertyFlags) {

      memoryTypeIndex = x;
      break;
    }
  }

  VkMemoryAllocateInfo memoryAllocateInfo = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = NULL,
      .allocationSize = memoryRequirements.size,
      .memoryTypeIndex = memoryTypeIndex};

  result = vkAllocateMemory(deviceHandleRef, &memoryAllocateInfo, NULL,
                            &this->deviceMemoryHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkAllocateMemory");
  }

  result = vkBindImageMemory(deviceHandleRef, this->imageHandle,
                             this->deviceMemoryHandle, 0);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkBindImageMemory");
  }
}

Image::~Image() {
  vkDestroyImage(this->deviceHandleRef, this->imageHandle, NULL);
  vkFreeMemory(this->deviceHandleRef, this->deviceMemoryHandle, NULL);
}

VkImage &Image::getImageHandleRef() { return this->imageHandle; }