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

#define LOAD_DEVICE_FUNCTION(deviceHandle, functionName,                \
    functionLocalName)                                                  \
  PFN##_##functionName functionLocalName =                              \
      (PFN##_##functionName)vkGetDeviceProcAddr(deviceHandle,           \
      #functionName);                                                   \
  if (functionLocalName == NULL)                                        \
    std::cerr << "Device funtion not found: " <<                        \
        #functionName << std::endl;

struct BaseVulkanStructure {
  VkStructureType sType;
  void* pNext;
};

void throwExceptionVulkanAPI(VkResult result, std::string functionName);
void throwExceptionMessage(std::string message);
