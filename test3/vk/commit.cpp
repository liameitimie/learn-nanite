#include "commit.h"
#include <vulkan/vulkan.h>
#include "vk_context.h"

namespace vk{

Result<Fence,Error> create_fence(bool signaled){
    VkFenceCreateInfo fence_desc={
        .sType=VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags=signaled?VK_FENCE_CREATE_SIGNALED_BIT:0u
    };
    VkFence fence;
    auto res=vkCreateFence((VkDevice)device(),&fence_desc,nullptr,&fence);
    if(res!=VK_SUCCESS) return Err((Error)res);
    return Ok(Fence{(u64)fence});
}

void wait_for_fence(Fence fence){
    vkWaitForFences((VkDevice)device(),1,(VkFence*)&fence.handle,VK_TRUE,(u64)(-1));
}

void reset_fence(Fence fence){
    vkResetFences((VkDevice)device(),1,(VkFence*)&fence.handle);
}

Result<Semaphore,Error> create_semaphore(){
    VkSemaphoreCreateInfo semaphore_desc={
        .sType=VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    VkSemaphore semaphore;
    auto res=vkCreateSemaphore((VkDevice)device(),&semaphore_desc,nullptr,&semaphore);
    if(res!=VK_SUCCESS) return Err((Error)res);
    return Ok(Semaphore{(u64)semaphore});
}

void queue_submit(SubmitInfo submit_info,Fence fence){
    vector<VkPipelineStageFlags> wait_stages(submit_info.waiting.size(),VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    vector<VkCommandBuffer> cmds;
    for(auto cmd: submit_info.command_buffers){
        cmds.push_back((VkCommandBuffer)cmd.handle);
    }
    VkSubmitInfo submit_desc={
        .sType=VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount=(u32)submit_info.waiting.size(),
        .pWaitSemaphores=(VkSemaphore*)submit_info.waiting.data(),
        .pWaitDstStageMask=wait_stages.data(),
        .commandBufferCount=(u32)submit_info.command_buffers.size(),
        .pCommandBuffers=cmds.data(),
        .signalSemaphoreCount=(u32)submit_info.signal.size(),
        .pSignalSemaphores=(VkSemaphore*)submit_info.signal.data()
    };
    vkQueueSubmit((VkQueue)queue(),1,&submit_desc,(VkFence)fence.handle);
}

void queue_present(PresentInfo present_info){
    VkSwapchainKHR sc=(VkSwapchainKHR)swapchain();
    VkPresentInfoKHR present_desc={
        .sType=VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount=(u32)present_info.waiting.size(),
        .pWaitSemaphores=(VkSemaphore*)present_info.waiting.data(),
        .swapchainCount=1,
        .pSwapchains=&sc,
        .pImageIndices=&present_info.swapchain_image_idx
    };
    vkQueuePresentKHR((VkQueue)queue(),&present_desc);
}

}