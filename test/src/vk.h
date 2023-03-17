#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

void init_glfw();
VkInstance create_instance(bool enable_validation);
VkPhysicalDevice get_adapter(int idx);
VkDevice create_device(VkPhysicalDevice adapter);
VkQueue get_queue(VkDevice device);
uint32_t get_queue_index();

VkSwapchainKHR create_swapchain(VkDevice dev,VkSurfaceKHR surface,uint32_t w,uint32_t h,VkFormat format);

void cleanup();