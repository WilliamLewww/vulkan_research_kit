#include "vrk/buffer.h"

Buffer::Buffer(VkDevice &deviceHandleRef,
               VkPhysicalDevice &physicalDeviceHandleRef,
               VkBufferCreateFlags bufferCreateFlags,
               VkDeviceSize bufferDeviceSize,
               VkBufferUsageFlags bufferUsageFlags, VkSharingMode sharingMode,
               std::vector<uint32_t> queueFamilyIndexList,
               VkMemoryPropertyFlags memoryPropertyFlags,
               std::vector<void *> memoryAllocateInfoChainList)
    : deviceHandleRef(deviceHandleRef) {

  this->bufferHandle = VK_NULL_HANDLE;

  this->bufferDeviceSize = bufferDeviceSize;

  VkBufferCreateInfo bufferCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = NULL,
      .flags = bufferCreateFlags,
      .size = bufferDeviceSize,
      .usage = bufferUsageFlags,
      .sharingMode = sharingMode,
      .queueFamilyIndexCount = (uint32_t)queueFamilyIndexList.size(),
      .pQueueFamilyIndices = queueFamilyIndexList.data()};

  VkResult result = vkCreateBuffer(deviceHandleRef, &bufferCreateInfo, NULL,
                                   &this->bufferHandle);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateBuffer");
  }

  VkMemoryRequirements memoryRequirements;
  vkGetBufferMemoryRequirements(deviceHandleRef, this->bufferHandle,
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

  if (memoryAllocateInfoChainList.size() > 0) {
    BaseVulkanStructure *previousStructure =
        (BaseVulkanStructure *)&memoryAllocateInfo;

    for (uint32_t x = 0; x < memoryAllocateInfoChainList.size(); x++) {
      BaseVulkanStructure *currentStructure =
          (BaseVulkanStructure *)memoryAllocateInfoChainList[x];

      previousStructure->pNext = currentStructure;
      previousStructure = currentStructure;
    }
  }

  result = vkAllocateMemory(deviceHandleRef, &memoryAllocateInfo, NULL,
                            &this->deviceMemoryHandle);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkAllocateMemory");
  }

  result = vkBindBufferMemory(deviceHandleRef, this->bufferHandle,
                              this->deviceMemoryHandle, 0);
  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkBindBufferMemory");
  }
}

Buffer::~Buffer() {
  vkDestroyBuffer(this->deviceHandleRef, this->bufferHandle, NULL);
  vkFreeMemory(this->deviceHandleRef, this->deviceMemoryHandle, NULL);
}

void Buffer::copyFromImageCmd(
    VkCommandBuffer &commandBufferHandleRef, VkImage &imageHandleRef,
    VkImageLayout imageLayout,
    std::vector<VkBufferImageCopy> bufferImageCopyList) {

  vkCmdCopyImageToBuffer(
      commandBufferHandleRef, imageHandleRef, imageLayout, this->bufferHandle,
      (uint32_t)bufferImageCopyList.size(), bufferImageCopyList.data());
}

void Buffer::copyFromBufferCmd(VkCommandBuffer &commandBufferHandleRef,
                               VkBuffer &srcBufferHandleRef,
                               VkBuffer &dstBufferHandleRef,
                               std::vector<VkBufferCopy> bufferCopyList) {

  vkCmdCopyBuffer(commandBufferHandleRef, srcBufferHandleRef,
                  dstBufferHandleRef, bufferCopyList.size(),
                  bufferCopyList.data());
}

void Buffer::mapMemory(void **hostMemory, VkDeviceSize offset,
                       VkDeviceSize size) {

  VkResult result = vkMapMemory(this->deviceHandleRef, this->deviceMemoryHandle,
                                offset, size, 0, hostMemory);

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkMapMemory");
  }
}

void Buffer::unmapMemory() {
  vkUnmapMemory(this->deviceHandleRef, this->deviceMemoryHandle);
}

void Buffer::bindVertexBufferCmd(VkCommandBuffer &commandBufferHandleRef,
                                 uint32_t binding) {

  VkDeviceSize offset = {0};
  vkCmdBindVertexBuffers(commandBufferHandleRef, binding, 1,
                         &this->bufferHandle, &offset);
}

void Buffer::bindIndexBufferCmd(VkCommandBuffer &commandBufferHandleRef,
                                VkIndexType indexType) {

  vkCmdBindIndexBuffer(commandBufferHandleRef, this->bufferHandle, 0,
                       indexType);
}

VkDeviceAddress Buffer::getBufferDeviceAddress() {
  VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
      .pNext = NULL,
      .buffer = this->bufferHandle};

  LOAD_DEVICE_FUNCTION(this->deviceHandleRef, vkGetBufferDeviceAddressKHR,
                       pvkGetBufferDeviceAddressKHR);

  return pvkGetBufferDeviceAddressKHR(this->deviceHandleRef,
                                      &bufferDeviceAddressInfo);
}

VkBuffer &Buffer::getBufferHandleRef() { return this->bufferHandle; }
