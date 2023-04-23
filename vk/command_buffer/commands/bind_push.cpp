#include "bind_push.h"
#include <vulkan/vulkan.h>

namespace vk{

void bind_graphics_pipeline(u64 cmd,u64 pipeline){
    vkCmdBindPipeline((VkCommandBuffer)cmd,VK_PIPELINE_BIND_POINT_GRAPHICS,(VkPipeline)pipeline);
}

void bind_compute_pipeline(u64 cmd,u64 pipeline){
    vkCmdBindPipeline((VkCommandBuffer)cmd,VK_PIPELINE_BIND_POINT_COMPUTE,(VkPipeline)pipeline);
}

void bind_vertex_buffer(u64 cmd,vector<Buffer> buffers){
    vector<VkBuffer> vkbuffers;
    vector<u64> offsets;
    for(auto buffer: buffers){
        vkbuffers.push_back((VkBuffer)buffer.handle);
        offsets.push_back(0);
    }
    vkCmdBindVertexBuffers((VkCommandBuffer)cmd,0,buffers.size(),vkbuffers.data(),offsets.data());
}

void bind_index_buffer(u64 cmd,Buffer index_buffer){
    VkIndexType t=VK_INDEX_TYPE_UINT32;
    if(index_buffer.stride==2){
        t=VK_INDEX_TYPE_UINT16;
    }
    vkCmdBindIndexBuffer((VkCommandBuffer)cmd,(VkBuffer)index_buffer.handle,0,t);
}

void push_constant(u64 cmd,u64 layout,u32 size,void* p){
    vkCmdPushConstants(
        (VkCommandBuffer)cmd,
        (VkPipelineLayout)layout,
        VK_SHADER_STAGE_ALL_GRAPHICS|VK_SHADER_STAGE_COMPUTE_BIT,
        0,
        size,
        p
    );
}

void bind_descriptor_sets(u64 cmd,PipelineBindPoint binding_point,u64 layout,u32 idx,u64 desc_set){
    vkCmdBindDescriptorSets(
        (VkCommandBuffer)cmd,
        (VkPipelineBindPoint)binding_point,
        (VkPipelineLayout)layout,
        idx,
        1,
        (VkDescriptorSet*)&desc_set,
        0,
        nullptr);
}

}