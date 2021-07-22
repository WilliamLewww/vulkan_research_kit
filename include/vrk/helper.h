#pragma once

#include <vulkan/vulkan.h>

#define STRING_RESET    "\033[0m"
#define STRING_INFO     "\033[37m"
#define STRING_WARNING  "\033[33m"
#define STRING_ERROR    "\033[36m"

#define PRINT_RETURN_CODE(stream, code, functionName)                   \
  stream << "Return code " << code <<                                   \
      " (" << functionName << ")" << std::endl

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
