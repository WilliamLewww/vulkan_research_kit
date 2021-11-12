#pragma once

#include <string>
#include <vector>

const std::vector<std::string> RAY_QUERY_REQUIRED_INSTANCE_EXTENSION_LIST = {
    "VK_KHR_get_physical_device_properties2"};

const std::vector<std::string> RAY_QUERY_REQUIRED_DEVICE_EXTENSION_LIST = {
    "VK_KHR_ray_query",
    "VK_KHR_spirv_1_4",
    "VK_KHR_shader_float_controls",
    "VK_KHR_acceleration_structure",
    "VK_EXT_descriptor_indexing",
    "VK_KHR_maintenance3",
    "VK_KHR_buffer_device_address",
    "VK_KHR_deferred_host_operations"};

const std::vector<std::string>
    RAY_TRACING_PIPELINE_REQUIRED_INSTANCE_EXTENSION_LIST = {
        "VK_KHR_get_physical_device_properties2"};

const std::vector<std::string>
    RAY_TRACING_PIPELINE_REQUIRED_DEVICE_EXTENSION_LIST = {
        "VK_KHR_ray_tracing_pipeline",  "VK_KHR_acceleration_structure",
        "VK_EXT_descriptor_indexing",   "VK_KHR_maintenance3",
        "VK_KHR_buffer_device_address", "VK_KHR_deferred_host_operations"};
