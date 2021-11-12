#pragma once

#include <string>
#include <vector>

const std::vector<std::string>
    DYNAMIC_RENDERING_REQUIRED_INSTANCE_EXTENSION_LIST = {
        "VK_KHR_get_physical_device_properties2"};

const std::vector<std::string>
    DYNAMIC_RENDERING_REQUIRED_DEVICE_EXTENSION_LIST = {
        "VK_KHR_dynamic_rendering"};
