#include "set_state.h"
#include <vulkan/vulkan.h>

namespace vk{

void set_viewport(u64 cmd,vector<Viewport> viewports){
    vector<VkViewport> vk_viewports;
    for(auto v:viewports){
        vk_viewports.push_back(VkViewport{
            .x=v.origin.x,
            .y=v.origin.y,
            .width=v.dimensions.x,
            .height=v.dimensions.y,
            .minDepth=v.depth_range.x,
            .maxDepth=v.depth_range.y
        });
    }
    vkCmdSetViewport((VkCommandBuffer)cmd,0,vk_viewports.size(),vk_viewports.data());
}

}