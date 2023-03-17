#include "command_buffer_allocator.h"
#include <vulkan/vulkan.h>
#include "../vk_context.h"

namespace vk{

auto CommandBufferAllocator::new_()->Result<CommandBufferAllocator,Error>{
    VkCommandPool vk_command_pool;
    VkCommandPoolCreateInfo cmd_pool_desc={
        .sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags=VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex=queue_family(),
    };
    auto res=vkCreateCommandPool((VkDevice)device(),&cmd_pool_desc,nullptr,&vk_command_pool);
    if(res!=VK_SUCCESS) return Err((Error)res);

    return Ok(CommandBufferAllocator{(u64)vk_command_pool});
}

}