#include "render_pass.h"
#include <vulkan/vulkan.h>
#include "../../vk_context.h"

namespace vk{

VkRenderingAttachmentInfo transform(RenderingAttachmentInfo info){
    return VkRenderingAttachmentInfo {
        .sType=VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView=(VkImageView)info.image_view,
        .imageLayout=VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
        .loadOp=(VkAttachmentLoadOp)info.load_op,
        .storeOp=(VkAttachmentStoreOp)info.store_op,
        //.clearValue=*((VkClearValue*)&info.clear_value),
    };
}

void begin_rendering(u64 cmd,RenderingInfo rendering_info){
    vector<VkRenderingAttachmentInfo> vkc;
    vector<VkRenderingAttachmentInfo> vkd;
    vector<VkRenderingAttachmentInfo> vks;

    for(RenderingAttachmentInfo& info: rendering_info.color_attachments){
        vkc.push_back(transform(info));
    }
    if(rendering_info.depth_attachment.is_some()){
        vkd.push_back(transform(rendering_info.depth_attachment.unwrap()));
    }
    if(rendering_info.stencil_attachment.is_some()){
        vks.push_back(transform(rendering_info.stencil_attachment.unwrap()));
    }

    VkRenderingInfo vk_render_info={
        .sType=VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea={
            .offset{
                .x=rendering_info.render_area_offset.x,
                .y=rendering_info.render_area_offset.y
            },
            .extent{
                .width=rendering_info.render_area_extent.x,
                .height=rendering_info.render_area_extent.y
            }
        },
        .layerCount=1,
        .colorAttachmentCount=(u32)vkc.size(),
        .pColorAttachments=vkc.data(),
        .pDepthAttachment=vkd.size()?vkd.data():nullptr,
        .pStencilAttachment=vks.size()?vks.data():nullptr,
    };
    vkCmdBeginRendering((VkCommandBuffer)cmd,&vk_render_info);
}

void end_rendering(u64 cmd){
    vkCmdEndRendering((VkCommandBuffer)cmd);
}

}
