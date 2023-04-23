#include "barrier.h"
#include <vulkan/vulkan.h>
#include "../../image/aspect.h"

using namespace std;

namespace vk{

void pipeline_barrier(u64 cmd,Dependency dependency){
    vector<VkImageMemoryBarrier2> image_barriers;
    for(auto barrier:dependency.image_barriers){
        VkImageMemoryBarrier2 vk_barrier{
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
                .levelCount=barrier.image.mip_levels,
                .layerCount=1,
            }
        };
        if(barrier.sub_range.is_some()){
            auto sub_range=barrier.sub_range.unwrap();
            vk_barrier.subresourceRange.baseMipLevel=sub_range.base_mip_level;
            vk_barrier.subresourceRange.levelCount=sub_range.level_count;
        }
        image_barriers.push_back(vk_barrier);
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