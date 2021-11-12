#pragma once

const std::vector<std::string> SWAPCHAIN_REQUIRED_INSTANCE_EXTENSION_LIST = {
    "VK_KHR_surface"};

const std::vector<std::string> SWAPCHAIN_REQUIRED_DEVICE_EXTENSION_LIST = {
    "VK_KHR_swapchain"};

const std::vector<std::string> XLIB_SURFACE_REQUIRED_INSTANCE_EXTENSION_LIST = {
    "VK_KHR_xlib_surface", "VK_KHR_surface"};
