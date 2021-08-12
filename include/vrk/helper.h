#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <exception>
#include <string>

#define STRING_RESET    "\033[0m"
#define STRING_INFO     "\033[37m"
#define STRING_WARNING  "\033[33m"
#define STRING_ERROR    "\033[36m"

#define PRINT_MESSAGE(stream, message)                                  \
  stream << message << std::endl;

#define LOAD_INSTANCE_FUNCTION(instanceHandle, functionName,            \
    functionLocalName)                                                  \
  PFN##_##functionName functionLocalName =                              \
      (PFN##_##functionName)vkGetInstanceProcAddr(instanceHandle,       \
      #functionName);                                                   \
  if (functionLocalName == NULL)                                        \
    std::cerr << "Instance funtion not found: " <<                      \
        #functionName << std::endl;

void throwExceptionVulkanAPI(VkResult result, std::string functionName);
void throwExceptionMessage(std::string message);