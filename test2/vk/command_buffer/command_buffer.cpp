#include "command_buffer.h"
#include <vulkan/vulkan.h>
#include "../vk_context.h"
namespace vk{

auto CommandBuffer::new_()->Result<CommandBuffer,Error>{
    VkResult res;
    VkCommandPoolCreateInfo cmd_pool_desc={
        .sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex=queue_family(),
    };
    res=vkCreateCommandPool((VkDevice)device(),&cmd_pool_desc,nullptr,(VkCommandPool*)&command_pool);
    if(res!=VK_SUCCESS) return Err((Error)res);
    VkCommandBufferAllocateInfo cmd_buffer_alloc_desc={
        .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool=(VkCommandPool)command_pool,
        .level=VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount=1,
    };
    res=vkAllocateCommandBuffers((VkDevice)device(),&cmd_buffer_alloc_desc,(VkCommandBuffer*)&handle);
    if(res!=VK_SUCCESS) return Err((Error)res);
    VkCommandBufferBeginInfo cmd_begin_info={
        .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
       .flags=VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };
    res=vkBeginCommandBuffer((VkCommandBuffer)handle,&cmd_begin_info);
    if(res!=VK_SUCCESS) return Err((Error)res);
    return Ok(*this);
}

auto CommandBuffer::build()->Result<CommandBuffer,Error>{
    VkResult res=vkEndCommandBuffer((VkCommandBuffer)handle);
    if(res!=VK_SUCCESS) return Err((Error)res);
    return Ok(*this);
}

}