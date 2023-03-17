#include "bind_push.h"
#include <vulkan/vulkan.h>

namespace vk{

void bind_graphics_pipeline(u64 cmd,u64 pipeline){
    vkCmdBindPipeline((VkCommandBuffer)cmd,VK_PIPELINE_BIND_POINT_GRAPHICS,(VkPipeline)pipeline);
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

}