#include "barrier.h"
#include <vulkan/vulkan.h>
#include "../../image/aspect.h"

using namespace std;

namespace vk{

void pipeline_barrier(u64 cmd,Dependency dependency){
    vector<VkImageMemoryBarrier2> image_barriers;
    for(auto barrier:dependency.image_barriers){
        image_barriers.push_back(VkImageMemoryBarrier2{
            .sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask=(VkPipelineStageFlags2)barrier.src_stage,
            .srcAccessMask=(VkAccessFlags2)barrier.src_access,
            .dstStageMask=(VkPipelineStageFlags2)barrier.dst_stage,
            .dstAccessMask=(VkAccessFlags2)barrier.dst_access,
            .oldLayout=(VkImageLayout)barrier.old_layout,
            .newLayout=(VkImageLayout)barrier.new_layout,
            .image=(VkImage)barrier.image.handle,
            .subresourceRange={
                .aspectMask=(VkImageAspectFlags)aspect(barrier.image.format),
                .levelCount=1,
                .layerCount=1,
            }
        });
    }

    vector<VkBufferMemoryBarrier2> buffer_barriers;
    for(auto barrier:dependency.buffer_barriers){
        buffer_barriers.push_back(VkBufferMemoryBarrier2{
            .sType=VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .srcStageMask=(VkPipelineStageFlags2)barrier.src_stage,
            .srcAccessMask=(VkAccessFlags2)barrier.src_access,
            .dstStageMask=(VkPipelineStageFlags2)barrier.dst_stage,
            .dstAccessMask=(VkAccessFlags2)barrier.dst_access,
            .buffer=(VkBuffer)barrier.buffer.handle,
            .offset=0,
            .size=barrier.buffer.size
        });
    }

    VkDependencyInfo dep={
        .sType=VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .bufferMemoryBarrierCount=(u32)buffer_barriers.size(),
        .pBufferMemoryBarriers=buffer_barriers.data(),
        .imageMemoryBarrierCount=(u32)image_barriers.size(),
        .pImageMemoryBarriers=image_barriers.data(),
    };
    vkCmdPipelineBarrier2((VkCommandBuffer)cmd,&dep);
}

}