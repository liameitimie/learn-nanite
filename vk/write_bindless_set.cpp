#include "write_bindless_set.h"
#include "vk_context.h"
#include <vulkan/vulkan.h>

namespace vk{

void write_bindless_set(u32 idx,Buffer buffer,DescriptorType type){
    VkDescriptorBufferInfo buffer_info={
        .buffer=(VkBuffer)buffer.handle,
        .offset=0,
        .range=buffer.size
    };
    VkWriteDescriptorSet write_desc={
        .sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet=(VkDescriptorSet)bindless_set(),
        .dstArrayElement=idx,
        .descriptorCount=1,
        .descriptorType=(VkDescriptorType)type,
        .pBufferInfo=&buffer_info
    };
    vkUpdateDescriptorSets((VkDevice)device(),1,&write_desc,0,nullptr);
}


void write_bindless_set(u32 idx,Buffer* buffers,u32 buffer_count,DescriptorType type){
    vector<VkDescriptorBufferInfo> buffer_infos(buffer_count);
    u32 i=0;
    for(auto& buffer_info:buffer_infos){
        buffer_info=VkDescriptorBufferInfo{
            .buffer=(VkBuffer)buffers[i].handle,
            .offset=0,
            .range=buffers[i].size
        };
        i++;
    }
    VkWriteDescriptorSet write_desc={
        .sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet=(VkDescriptorSet)bindless_set(),
        .dstBinding=0,
        .dstArrayElement=idx,
        .descriptorCount=buffer_count,
        .descriptorType=(VkDescriptorType)type,
        .pBufferInfo=buffer_infos.data()
    };
    vkUpdateDescriptorSets((VkDevice)device(),1,&write_desc,0,nullptr);
}


}