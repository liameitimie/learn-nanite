#include "compute.h"
#include <vulkan/vulkan.h>
#include <vk_context.h>
#include <vk_check.h>

namespace vk{

auto ComputePipeline::build()->Result<ComputePipeline,Error>{
    VkPipelineLayout pipeline_layout;
    VkPushConstantRange push_desc={
        .stageFlags=VK_SHADER_STAGE_COMPUTE_BIT|VK_SHADER_STAGE_ALL_GRAPHICS,
        .offset=0,
        .size=push_constant_size_,
    };
    VkDescriptorSetLayout layouts[2]={
        VkDescriptorSetLayout(bindless_buffer_layout()),
        VkDescriptorSetLayout(bindless_image_layout())
    };
    auto layout_desc=VkPipelineLayoutCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount=2,
        .pSetLayouts=layouts,
        .pushConstantRangeCount=push_constant_size_?1u:0,
        .pPushConstantRanges=push_constant_size_?&push_desc:nullptr,
    };
    VK_CHECK(vkCreatePipelineLayout((VkDevice)device(),&layout_desc,nullptr,&pipeline_layout));

    VkComputePipelineCreateInfo ppl_info={
        .sType=VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage={
            .sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage=VK_SHADER_STAGE_COMPUTE_BIT,
            .module=(VkShaderModule)compute_shader_.handle,
            .pName="main"
        },
        .layout=pipeline_layout
    };
    VkPipeline pipeline;
    VK_CHECK(vkCreateComputePipelines((VkDevice)device(),VK_NULL_HANDLE,1,&ppl_info,nullptr,&pipeline));
    this->handle=(u64)pipeline;
    this->pipeline_layout=(u64)pipeline_layout;
    return Ok(*this);
}

}