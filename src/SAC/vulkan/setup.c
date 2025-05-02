#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "SAC/state/state.h"
#include "SAC/vulkan/setup.h"

int SetupVulkanWrapper(AppState *state) {
  // Setup Vulkan
  uint32_t extensions_count = 0;
  PGVulkan vulkan_state = &state->vulkan_globals;

  // TODO: see if the following is sufficient as an alternative, or needed
  // The lines after the next sets up Vulkan
  // vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, NULL);

  const char *const *extensions_nude = SDL_Vulkan_GetInstanceExtensions(&extensions_count);
  if (extensions_nude == NULL) {
    printf("Error: SDL_Vulkan_GetInstanceExtensions(): %s\n", SDL_GetError());
    return -1;
  }
  const char** extensions = (const char**)malloc(extensions_count * sizeof(const char*));
  if (extensions == NULL) {
    printf("Error allocating space for extensions array\n");
    return -1;
  }

  for (int i = 0; i < extensions_count; i++) {
    extensions[i] = extensions_nude[i];
  }

  SetupVulkan(extensions, extensions_count, vulkan_state);
  //leak?? but free crashes
  // free(extensions);
  // Create Window Surface
  VkSurfaceKHR surface;
  VkResult err;
  if (SDL_Vulkan_CreateSurface(state->window, vulkan_state->instance, vulkan_state->allocator, &surface) == 0) {
    printf("Failed to create Vulkan surface.\n");
    return 1;
  }

  printf("SETUP: %i extensions supported\n", extensions_count);

  // Create Framebuffers
  printf("SETUP: Create FrameBuffers\n");
  {
    int w, h;
    SDL_GetWindowSize(state->window, &w, &h);
    SetupVulkanWindow(vulkan_state, &vulkan_state->mainWindowData, surface, w, h);
    SDL_SetWindowPosition(state->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(state->window);
  }

  return 0;
}

void PopInitData(PGVulkan app_state) {
  app_state->allocator = NULL;
  app_state->instance = VK_NULL_HANDLE;
  app_state->physicalDevice = VK_NULL_HANDLE;
  app_state->device = VK_NULL_HANDLE;
  app_state->queueFamily = (uint32_t)-1;
  app_state->queue = VK_NULL_HANDLE;
  app_state->debugReport = VK_NULL_HANDLE;
  app_state->pipelineCache = VK_NULL_HANDLE;
  app_state->descriptorPool = VK_NULL_HANDLE;

  // g_MainWindowData.ClearEnable = true;
	// ImGui_ImplVulkanH_Window_Construct(&g_MainWindowData);
	// g_MainWindowData = *ImGui_ImplVulkanH_Window_ImGui_ImplVulkanH_Window();
  app_state->mainWindowData = *ImGui_ImplVulkanH_Window_ImGui_ImplVulkanH_Window();
  app_state->minImageCount = 2;
  app_state->swapChainRebuild = false;
}

void check_vk_result(VkResult err) {
  if (err == VK_SUCCESS)
    return;
  fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
  if (err < 0)
    abort();
}

void check_vk_result_line(VkResult err,int line) {
  if (err == VK_SUCCESS)
    return;
  fprintf(stderr, "[vulkan] Error: VkResult = %d in line: (%d)\n", err, line);
  if (err < 0)
    abort();
}

#ifdef APP_USE_VULKAN_DEBUG_REPORT
VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
  (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
  fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
  return VK_FALSE;
}
#endif // APP_USE_VULKAN_DEBUG_REPORT
//*
bool IsExtensionAvailable(VkExtensionProperties* properties, uint32_t properties_count, const char* extension) {
  for(uint32_t i = 0 ; i < properties_count; i++){
    if (strcmp(properties[i].extensionName, extension) == 0)
      return true;
  }
  return false;
}

void SetupVulkan(const char** extensions, uint32_t extensions_count, PGVulkan app_state) {
  VkResult err;
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
  volkInitialize();
#endif

  // Create Vulkan Instance
  {
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    // Enumerate available extensions
    uint32_t properties_count;
    vkEnumerateInstanceExtensionProperties(NULL, &properties_count, NULL);
    VkExtensionProperties* properties = (VkExtensionProperties*)malloc(properties_count * sizeof(VkExtensionProperties));
    err = vkEnumerateInstanceExtensionProperties(NULL, &properties_count, properties);
    check_vk_result_line(err, __LINE__);

    // Enable required extensions
    if (IsExtensionAvailable(properties, properties_count, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
      //instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
      extensions_count++;
      extensions = realloc(extensions, extensions_count * sizeof(char*));
      assert(extensions);
      extensions[extensions_count-1] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
    }
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
    if (IsExtensionAvailable(properties, properties_count, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
      //instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      extensions_count++;
      extensions = realloc(extensions, extensions_count * sizeof(char*));
      assert(extensions);
      extensions[extensions_count-1] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
      create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }
#endif
    free(properties);
    // Enabling validation layers
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
    create_info.enabledLayerCount = 1;
    create_info.ppEnabledLayerNames = layers;
    //instance_extensions.push_back("VK_EXT_debug_report");
    extensions_count++;
    extensions = realloc(extensions, extensions_count * sizeof(char*));
    assert(extensions);
    extensions[extensions_count-1] = "VK_EXT_debug_report";
#endif

    // Create Vulkan Instance
    create_info.enabledExtensionCount = extensions_count;//(uint32_t)instance_extensions.Size;
    create_info.ppEnabledExtensionNames = extensions;//instance_extensions.Data;
    err = vkCreateInstance(&create_info, app_state->allocator, &app_state->instance);
    check_vk_result_line(err, __LINE__);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
    volkLoadInstance(app_state->instance);
#endif

    // Setup the debug report callback
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    PFN_vkCreateDebugReportCallbackEXT f_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(app_state->instance, "vkCreateDebugReportCallbackEXT");
    IM_ASSERT(f_vkCreateDebugReportCallbackEXT != NULL);
    VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
    debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    debug_report_ci.pfnCallback = debug_report;
    debug_report_ci.pUserData = NULL;
    err = f_vkCreateDebugReportCallbackEXT(app_state->instance, &debug_report_ci, app_state->allocator, &app_state->debugReport);
    check_vk_result_line(err, __LINE__);
#endif
  }
  // Select Physical Device (GPU)
  app_state->physicalDevice = ImGui_ImplVulkanH_SelectPhysicalDevice(app_state->instance);
  IM_ASSERT(app_state->physicalDevice != VK_NULL_HANDLE);
  // Select graphics queue family
  app_state->queueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(app_state->physicalDevice);
  IM_ASSERT(app_state->queueFamily != (uint32_t)-1);

  // Create Logical Device (with 1 queue)
  {
    //ImVector<const char*> device_extensions;
    //device_extensions.push_back("VK_KHR_swapchain");
    uint32_t device_extensions_count = 1;
    const char** device_extensions = (const char**)malloc(device_extensions_count * sizeof(char*));
    device_extensions[0] = "VK_KHR_swapchain";

    // Enumerate physical device extension
    uint32_t properties_count;
    //ImVector<VkExtensionProperties> properties;
    vkEnumerateDeviceExtensionProperties(app_state->physicalDevice, NULL, &properties_count, NULL);
    //properties.resize(properties_count);
    VkExtensionProperties* properties = (VkExtensionProperties*)malloc(properties_count * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(app_state->physicalDevice, NULL, &properties_count, properties);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
    if (IsExtensionAvailable(properties, properties_count, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
      //device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
      device_extensions_count++;
      device_extensions = realloc(device_extensions, device_extensions_count * sizeof(const char*));
      assert(device_extensions);
      device_extensions[device_extensions_count-1] = VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
    }
#endif

    const float queue_priority[] = { 1.0f };
    VkDeviceQueueCreateInfo queue_info[1] = {};
    queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info[0].queueFamilyIndex = app_state->queueFamily;
    queue_info[0].queueCount = 1;
    queue_info[0].pQueuePriorities = queue_priority;
    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
    create_info.pQueueCreateInfos = queue_info;
    create_info.enabledExtensionCount = device_extensions_count; //(uint32_t)device_extensions.Size;
    create_info.ppEnabledExtensionNames = device_extensions;//device_extensions.Data;
    err = vkCreateDevice(app_state->physicalDevice, &create_info, app_state->allocator, &app_state->device);
    check_vk_result_line(err, __LINE__);
    vkGetDeviceQueue(app_state->device, app_state->queueFamily, 0, &app_state->queue);
  }
  // Create Descriptor Pool
  // If you wish to load e.g. additional textures you may need to alter pools sizes and maxSets.
  {
    VkDescriptorPoolSize pool_sizes[] = {
      { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE },
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 0;
    //for (VkDescriptorPoolSize& pool_size : pool_sizes)
    //  pool_info.maxSets += pool_size.descriptorCount;
    for(int i = 0; i < IM_ARRAYSIZE(pool_sizes); i++) {
      VkDescriptorPoolSize pool_size = pool_sizes[i];
      pool_info.maxSets += pool_size.descriptorCount;
    }
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    err = vkCreateDescriptorPool(app_state->device, &pool_info, app_state->allocator, &app_state->descriptorPool);
    check_vk_result_line(err, __LINE__);
  }
}
//*/
// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
void SetupVulkanWindow(PGVulkan app_state, ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height) {
  wd->Surface = surface;

  // Check for WSI support
  VkBool32 res;
  vkGetPhysicalDeviceSurfaceSupportKHR(app_state->physicalDevice, app_state->queueFamily, wd->Surface, &res);
  if (res != VK_TRUE) {
    fprintf(stderr, "Error no WSI support on physical device 0\n");
    exit(-1);
  }

  // Select Surface Format
  const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
  const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(app_state->physicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

  // Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
  VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
  VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
  wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(app_state->physicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
  //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

  // Create SwapChain, RenderPass, Framebuffer, etc.
  IM_ASSERT(app_state->minImageCount >= 2);
  ImGui_ImplVulkanH_CreateOrResizeWindow(app_state->instance, app_state->physicalDevice, app_state->device, wd, app_state->queueFamily, app_state->allocator, width, height, app_state->minImageCount);
}

void CleanupVulkan(PGVulkan app_state) {
  vkDestroyDescriptorPool(app_state->device, app_state->descriptorPool, app_state->allocator);

#ifdef APP_USE_VULKAN_DEBUG_REPORT
  // Remove the debug report callback
  PFN_vkDestroyDebugReportCallbackEXT f_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(app_state->instance, "vkDestroyDebugReportCallbackEXT");
  f_vkDestroyDebugReportCallbackEXT(app_state->instance, app_state->debugReport, app_state->allocator);
#endif // APP_USE_VULKAN_DEBUG_REPORT

  vkDestroyDevice(app_state->device, app_state->allocator);
  vkDestroyInstance(app_state->instance, app_state->allocator);
}

void CleanupVulkanWindow(PGVulkan app_state) {
  ImGui_ImplVulkanH_DestroyWindow(app_state->instance, app_state->device, &app_state->mainWindowData, app_state->allocator);
}

void FrameRender(PGVulkan app_state, ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data) {
  VkSemaphore image_acquired_semaphore  = wd->FrameSemaphores.Data[wd->SemaphoreIndex].ImageAcquiredSemaphore;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores.Data[wd->SemaphoreIndex].RenderCompleteSemaphore;
  VkResult err = vkAcquireNextImageKHR(app_state->device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    app_state->swapChainRebuild = true;
  if (err == VK_ERROR_OUT_OF_DATE_KHR)
    return;
  if (err != VK_SUBOPTIMAL_KHR)
    check_vk_result_line(err, __LINE__);

  ImGui_ImplVulkanH_Frame* fd = &wd->Frames.Data[wd->FrameIndex];
  {
    err = vkWaitForFences(app_state->device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
    check_vk_result_line(err, __LINE__);

    err = vkResetFences(app_state->device, 1, &fd->Fence);
    check_vk_result_line(err, __LINE__);
  }
  {
    err = vkResetCommandPool(app_state->device, fd->CommandPool, 0);
    check_vk_result_line(err, __LINE__);
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
    check_vk_result_line(err, __LINE__);
  }
  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = wd->RenderPass;
    info.framebuffer = fd->Framebuffer;
    info.renderArea.extent.width = wd->Width;
    info.renderArea.extent.height = wd->Height;
    info.clearValueCount = 1;
    info.pClearValues = &wd->ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  // Record dear imgui primitives into command buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer, VK_NULL_HANDLE);

  // Submit command buffer
  vkCmdEndRenderPass(fd->CommandBuffer);
  {
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_acquired_semaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &fd->CommandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphore;

    err = vkEndCommandBuffer(fd->CommandBuffer);
    check_vk_result_line(err, __LINE__);
    err = vkQueueSubmit(app_state->queue, 1, &info, fd->Fence);
    check_vk_result_line(err, __LINE__);
  }
}

void FramePresent(PGVulkan app_state, ImGui_ImplVulkanH_Window* wd) {
  if (app_state->swapChainRebuild)
    return;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores.Data[wd->SemaphoreIndex].RenderCompleteSemaphore;
  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &wd->Swapchain;
  info.pImageIndices = &wd->FrameIndex;
  VkResult err = vkQueuePresentKHR(app_state->queue, &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    app_state->swapChainRebuild = true;
  if (err == VK_ERROR_OUT_OF_DATE_KHR)
    return;
  if (err != VK_SUBOPTIMAL_KHR)
    check_vk_result_line(err, __LINE__);
  wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
}

