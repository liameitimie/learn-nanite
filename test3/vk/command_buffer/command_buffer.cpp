#include "command_buffer.h"
#include <vulkan/vulkan.h>
#include "../vk_context.h"
#include <vk_check.h>
namespace vk{

auto CommandBuffer::new_(
    CommandBufferAllocator allocator
)->Result<CommandBuffer,Error>{
    CommandBuffer cmd{0,allocator};

    VkCommandBufferAllocateInfo cmd_buffer_alloc_desc={
        .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool=(VkCommandPool)allocator.handle,
        .level=VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount=1,
    };
    auto res=vkAllocateCommandBuffers((VkDevice)device(),&cmd_buffer_alloc_desc,(VkCommandBuffer*)&cmd.handle);
    if(res!=VK_SUCCESS) return Err((Error)res);
    
    return Ok(cmd);
}

auto CommandBuffer::begin(CommandBufferUsage usage)->Result<CommandBuffer,Error>{
    VkCommandBufferBeginInfo cmd_begin_info={
        .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags=(VkCommandBufferUsageFlags)usage,
    };
    auto res=vkBeginCommandBuffer((VkCommandBuffer)handle,&cmd_begin_info);
    if(res!=VK_SUCCESS) return Err((Error)res);
    return Ok(*this);
}

auto CommandBuffer::build()->Result<CommandBuffer,Error>{
    VkResult res=vkEndCommandBuffer((VkCommandBuffer)handle);
    if(res!=VK_SUCCESS) return Err((Error)res);
    return Ok(*this);
}

auto CommandBuffer::reset()->Result<CommandBuffer,Error>{
    auto res=vkResetCommandBuffer((VkCommandBuffer)handle,0);
    if(res!=VK_SUCCESS) return Err((Error)res);
    return Ok(*this);
}

}