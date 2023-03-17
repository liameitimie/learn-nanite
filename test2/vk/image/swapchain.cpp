#include "swapchain.h"
#include <vulkan/vulkan.h>
#include "../vk_context.h"

namespace vk{

void acquire_next_image(Option<Semaphore> semaphore,Option<Fence> fence,u32& image_idx){
    vkAcquireNextImageKHR(
        (VkDevice)device(),
        (VkSwapchainKHR)swapchain(),
        (u64)(-1),
        semaphore.is_some()?(VkSemaphore)semaphore.unwrap().handle:nullptr,
        fence.is_some()?(VkFence)fence.unwrap().handle:nullptr,
        &image_idx);
}

}