#include "command_buffer.h"
#include <vulkan/vulkan.h>
#include "../vk_context.h"
#include "../image/aspect.h"
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

auto CommandBuffer::blit_image(Image img1,Image img2,ivec4 img1_range,ivec4 img2_range)->CommandBuffer{
    VkImageBlit blit{
        .srcSubresource={
            .aspectMask=aspect(img1.format),
            .layerCount=1,
        },
        .srcOffsets={
            VkOffset3D{img1_range.x,img1_range.y,0},
            VkOffset3D{img1_range.z,img1_range.w,1},
        },
        .dstSubresource={
            .aspectMask=aspect(img2.format),
            .layerCount=1
        },
        .dstOffsets={
            VkOffset3D{img2_range.x,img2_range.y,0},
            VkOffset3D{img2_range.z,img2_range.w,1},
        }
    };
    vkCmdBlitImage(
        (VkCommandBuffer)handle,
        (VkImage)img1.handle,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        (VkImage)img2.handle,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,&blit,VK_FILTER_LINEAR);
    return *this;
}

}