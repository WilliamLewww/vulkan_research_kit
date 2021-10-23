#include "vrk/ray_tracing/ray_tracing_pipeline_group.h"

RayTracingPipelineGroup::RayTracingPipelineGroup(
    VkDevice &deviceHandleRef, std::vector<RayTracingPipelineCreateInfoParam>
                                   rayTracingPipelineCreateInfoParamList)
    : deviceHandleRef(deviceHandleRef) {

  this->pipelineHandleList = std::vector<VkPipeline>(
      rayTracingPipelineCreateInfoParamList.size(), VK_NULL_HANDLE);

  std::vector<VkRayTracingPipelineCreateInfoKHR>
      rayTracingPipelineCreateInfoList = {};

  std::vector<std::vector<VkPipelineShaderStageCreateInfo>>
      pipelineShaderStageCreateInfoMainList = {};

  std::vector<std::vector<VkRayTracingShaderGroupCreateInfoKHR>>
      rayTracingShaderGroupCreateInfoMainList = {};

  std::vector<VkPipelineLibraryCreateInfoKHR> pipelineLibraryCreateInfoList =
      {};

  std::vector<VkRayTracingPipelineInterfaceCreateInfoKHR>
      rayTracingPipelineInterfaceCreateInfoList = {};

  std::vector<VkPipelineDynamicStateCreateInfo>
      pipelineDynamicStateCreateInfoList = {};

  for (RayTracingPipelineCreateInfoParam &rayTracingPipelineCreateInfoParam :
       rayTracingPipelineCreateInfoParamList) {

    std::vector<VkPipelineShaderStageCreateInfo>
        pipelineShaderStageCreateInfoList = {};

    for (PipelineShaderStageCreateInfoParam
             &pipelineShaderStageCreateInfoParam :
         rayTracingPipelineCreateInfoParam
             .pipelineShaderStageCreateInfoParamList) {

      VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
          .pNext = NULL,
          .flags =
              pipelineShaderStageCreateInfoParam.pipelineShaderStageCreateFlags,
          .stage = pipelineShaderStageCreateInfoParam.shaderStageFlagBits,
          .module = pipelineShaderStageCreateInfoParam.shaderModuleHandleRef,
          .pName = pipelineShaderStageCreateInfoParam.entryPointName.c_str(),
          .pSpecializationInfo =
              pipelineShaderStageCreateInfoParam.specializationInfoPtr.get()};

      pipelineShaderStageCreateInfoList.push_back(
          pipelineShaderStageCreateInfo);
    }

    pipelineShaderStageCreateInfoMainList.push_back(
        pipelineShaderStageCreateInfoList);

    std::vector<VkRayTracingShaderGroupCreateInfoKHR>
        rayTracingShaderGroupCreateInfoList = {};

    for (RayTracingShaderGroupCreateInfoParam
             &rayTracingShaderGroupCreateInfoParam :
         rayTracingPipelineCreateInfoParam
             .rayTracingShaderGroupCreateInfoParamList) {

      VkRayTracingShaderGroupCreateInfoKHR rayTracingShaderGroupCreateInfo = {
          .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
          .pNext = NULL,
          .type =
              rayTracingShaderGroupCreateInfoParam.rayTracingShaderGroupType,
          .generalShader = rayTracingShaderGroupCreateInfoParam.generalShader,
          .closestHitShader =
              rayTracingShaderGroupCreateInfoParam.closestHitShader,
          .anyHitShader = rayTracingShaderGroupCreateInfoParam.anyHitShader,
          .intersectionShader =
              rayTracingShaderGroupCreateInfoParam.intersectionShader,
          .pShaderGroupCaptureReplayHandle =
              rayTracingShaderGroupCreateInfoParam
                  .shaderGroupCaptureReplayHandlePtr.get()};

      rayTracingShaderGroupCreateInfoList.push_back(
          rayTracingShaderGroupCreateInfo);
    }

    rayTracingShaderGroupCreateInfoMainList.push_back(
        rayTracingShaderGroupCreateInfoList);

    VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = rayTracingPipelineCreateInfoParam.pipelineCreateFlags,
        .stageCount = (uint32_t)pipelineShaderStageCreateInfoMainList
                          [pipelineShaderStageCreateInfoMainList.size() - 1]
                              .size(),
        .pStages = pipelineShaderStageCreateInfoMainList
                       [pipelineShaderStageCreateInfoMainList.size() - 1]
                           .data(),
        .groupCount = (uint32_t)rayTracingShaderGroupCreateInfoMainList
                          [rayTracingShaderGroupCreateInfoMainList.size() - 1]
                              .size(),
        .pGroups = rayTracingShaderGroupCreateInfoMainList
                       [rayTracingShaderGroupCreateInfoMainList.size() - 1]
                           .data(),
        .maxPipelineRayRecursionDepth =
            rayTracingPipelineCreateInfoParam.maxPipelineRayRecursionDepth,
        .pLibraryInfo = NULL,
        .pLibraryInterface = NULL,
        .pDynamicState = NULL,
        .layout = rayTracingPipelineCreateInfoParam.pipelineLayoutHandleRef,
        .basePipelineHandle =
            rayTracingPipelineCreateInfoParam.basePipelineHandle,
        .basePipelineIndex =
            rayTracingPipelineCreateInfoParam.basePipelineIndex};

    if (rayTracingPipelineCreateInfoParam.pipelineLibraryCreateInfoParamPtr) {

      std::shared_ptr<PipelineLibraryCreateInfoParam>
          pipelineLibraryCreateInfoParamPtr =
              rayTracingPipelineCreateInfoParam
                  .pipelineLibraryCreateInfoParamPtr;

      VkPipelineLibraryCreateInfoKHR pipelineLibraryCreateInfo = {
          .sType = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR,
          .pNext = NULL,
          .libraryCount = (uint32_t)pipelineLibraryCreateInfoParamPtr
                              ->pipelineHandleList.size(),
          .pLibraries =
              pipelineLibraryCreateInfoParamPtr->pipelineHandleList.data()};

      pipelineLibraryCreateInfoList.push_back(pipelineLibraryCreateInfo);

      rayTracingPipelineCreateInfo.pLibraryInfo =
          &pipelineLibraryCreateInfoList[pipelineLibraryCreateInfoList.size() -
                                         1];
    }

    if (rayTracingPipelineCreateInfoParam
            .rayTracingPipelineInterfaceCreateInfoParamPtr) {

      std::shared_ptr<RayTracingPipelineInterfaceCreateInfoParam>
          rayTracingPipelineInterfaceCreateInfoParamPtr =
              rayTracingPipelineCreateInfoParam
                  .rayTracingPipelineInterfaceCreateInfoParamPtr;

      VkRayTracingPipelineInterfaceCreateInfoKHR
          rayTracingPipelineInterfaceCreateInfo = {
              .sType =
                  VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_INTERFACE_CREATE_INFO_KHR,
              .pNext = NULL,
              .maxPipelineRayPayloadSize =
                  rayTracingPipelineInterfaceCreateInfoParamPtr
                      ->maxPipelineRayPayloadSize,
              .maxPipelineRayHitAttributeSize =
                  rayTracingPipelineInterfaceCreateInfoParamPtr
                      ->maxPipelineRayHitAttributeSize};

      rayTracingPipelineInterfaceCreateInfoList.push_back(
          rayTracingPipelineInterfaceCreateInfo);

      rayTracingPipelineCreateInfo.pLibraryInterface =
          &rayTracingPipelineInterfaceCreateInfoList
              [rayTracingPipelineInterfaceCreateInfoList.size() - 1];
    }

    if (rayTracingPipelineCreateInfoParam
            .pipelineDynamicStateCreateInfoParamPtr) {

      std::shared_ptr<PipelineDynamicStateCreateInfoParam>
          pipelineDynamicStateCreateInfoParamPtr =
              rayTracingPipelineCreateInfoParam
                  .pipelineDynamicStateCreateInfoParamPtr;

      VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {

          .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .dynamicStateCount = (uint32_t)pipelineDynamicStateCreateInfoParamPtr
                                   ->dynamicStateList.size(),
          .pDynamicStates =
              pipelineDynamicStateCreateInfoParamPtr->dynamicStateList.data()};

      pipelineDynamicStateCreateInfoList.push_back(
          pipelineDynamicStateCreateInfo);

      rayTracingPipelineCreateInfo.pDynamicState =
          &pipelineDynamicStateCreateInfoList
              [pipelineDynamicStateCreateInfoList.size() - 1];
    }

    rayTracingPipelineCreateInfoList.push_back(rayTracingPipelineCreateInfo);
  }

  LOAD_DEVICE_FUNCTION(deviceHandleRef, vkCreateRayTracingPipelinesKHR,
                       pvkCreateRayTracingPipelinesKHR);

  VkResult result = pvkCreateRayTracingPipelinesKHR(
      deviceHandleRef, VK_NULL_HANDLE, VK_NULL_HANDLE,
      (uint32_t)rayTracingPipelineCreateInfoList.size(),
      rayTracingPipelineCreateInfoList.data(), NULL,
      this->pipelineHandleList.data());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkCreateRayTracingPipelinesKHR");
  }
}

RayTracingPipelineGroup::~RayTracingPipelineGroup() {
  for (VkPipeline &pipeline : this->pipelineHandleList) {
    vkDestroyPipeline(this->deviceHandleRef, pipeline, NULL);
  }
}

void RayTracingPipelineGroup::getRayTracingShaderGroupHandles(
    uint32_t pipelineIndex, uint32_t firstGroup, uint32_t groupCount,
    uint32_t dataSize, std::shared_ptr<void> dataPtr) {

  LOAD_DEVICE_FUNCTION(deviceHandleRef, vkGetRayTracingShaderGroupHandlesKHR,
                       pvkGetRayTracingShaderGroupHandlesKHR);

  VkResult result = pvkGetRayTracingShaderGroupHandlesKHR(
      this->deviceHandleRef, this->pipelineHandleList[pipelineIndex],
      firstGroup, groupCount, dataSize, dataPtr.get());

  if (result != VK_SUCCESS) {
    throwExceptionVulkanAPI(result, "vkGetRayTracingShaderGroupHandlesKHR");
  }
}

void RayTracingPipelineGroup::bindPipelineCmd(
    uint32_t pipelineIndex, VkCommandBuffer &commandBufferHandleRef) {

  vkCmdBindPipeline(commandBufferHandleRef,
                    VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
                    this->pipelineHandleList[pipelineIndex]);
}

void RayTracingPipelineGroup::traceRaysCmd(
    VkCommandBuffer &commandBufferHandleRef,
    std::shared_ptr<VkStridedDeviceAddressRegionKHR>
        raygenShaderBindingTablePtr,
    std::shared_ptr<VkStridedDeviceAddressRegionKHR> missShaderBindingTablePtr,
    std::shared_ptr<VkStridedDeviceAddressRegionKHR> hitShaderBindingTablePtr,
    std::shared_ptr<VkStridedDeviceAddressRegionKHR>
        callableShaderBindingTablePtr,
    uint32_t width, uint32_t height, uint32_t depth) {

  LOAD_DEVICE_FUNCTION(deviceHandleRef, vkCmdTraceRaysKHR, pvkCmdTraceRaysKHR);

  pvkCmdTraceRaysKHR(commandBufferHandleRef, raygenShaderBindingTablePtr.get(),
                     missShaderBindingTablePtr.get(),
                     hitShaderBindingTablePtr.get(),
                     callableShaderBindingTablePtr.get(), width, height, depth);
}
