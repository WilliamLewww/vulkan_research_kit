#include "vrk/helper.h"

void throwExceptionVulkanAPI(VkResult result, std::string functionName) {
  std::string message = "Vulkan API exception: return code " +
                        std::to_string(result) + " (" + functionName + ")";

  throw std::runtime_error(message);
}

void throwExceptionMessage(std::string message) {
  throw std::runtime_error(message);
}