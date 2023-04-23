#include "graphics.h"
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>
#include "../vk_context.h"
#include <vk_check.h>

namespace vk{

inline auto proc_shader_info(
    ShaderModule shader,
    VkShaderStageFlagBits shader_stage
)->VkPipelineShaderStageCreateInfo{
    return VkPipelineShaderStageCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage=shader_stage,
        .module=(VkShaderModule)shader.handle,
        .pName="main"
    };
};

auto proc_vertex_input(
    vector<VertexBufferDescription> vertex_input_state
)->pair<vector<VkVertexInputBindingDescription>,vector<VkVertexInputAttributeDescription>>{

    vector<VkVertexInputBindingDescription> bindings;
    vector<VkVertexInputAttributeDescription> attributes;

    u32 binding_id=0;
    u32 location_id=0;
    for(auto& vdesc: vertex_input_state){
        bindings.push_back(VkVertexInputBindingDescription{
            .binding=binding_id,
            .stride=vdesc.stride,
            .inputRate=(VkVertexInputRate)vdesc.input_rate
        });
        for(auto& mdesc: vdesc.member){
            attributes.push_back(VkVertexInputAttributeDescription{
                .location=location_id,
                .binding=binding_id,
                .format=(VkFormat)mdesc.format,
                .offset=mdesc.offset
            });
            location_id++;
        }
        binding_id++;
    }
    return {bindings,attributes};
}

auto proc_vertex_input_desc(
    vector<VkVertexInputBindingDescription> &bindings,
    vector<VkVertexInputAttributeDescription> &attributes
)->VkPipelineVertexInputStateCreateInfo{
    return VkPipelineVertexInputStateCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount=(u32)bindings.size(),
        .pVertexBindingDescriptions=bindings.data(),
        .vertexAttributeDescriptionCount=(u32)attributes.size(),
        .pVertexAttributeDescriptions=attributes.data()
    };
}

auto proc_input_assembly(
    InputAssemblyState input_assembly_state
)->VkPipelineInputAssemblyStateCreateInfo{
    return VkPipelineInputAssemblyStateCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology=(VkPrimitiveTopology)input_assembly_state.topology,
    };
}

auto proc_viewport(
    ViewportState viewport_state
)->pair<vector<VkViewport>,vector<VkRect2D>>{
    vector<VkViewport> viewports;
    vector<VkRect2D> scissors;
    for(auto [viewport,scissor]: viewport_state.data){
        viewports.push_back(VkViewport{
            .x=viewport.origin.x,
            .y=viewport.origin.y,
            .width=viewport.dimensions.x,
            .height=viewport.dimensions.y,
            .minDepth=viewport.depth_range.x,
            .maxDepth=viewport.depth_range.y,
        });
        scissors.push_back(VkRect2D{
            .offset={
                .x=scissor.origin.x,
                .y=scissor.origin.y
            },
            .extent={
                .width=scissor.dimensions.x,
                .height=scissor.dimensions.y
            }
        });
    }
    return {viewports,scissors};
}
auto proc_viewport_desc(
    vector<VkViewport>& viewports,
    vector<VkRect2D>& scissors
)->VkPipelineViewportStateCreateInfo{
    return VkPipelineViewportStateCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount=(u32)viewports.size(),
        .pViewports=viewports.data(),
        .scissorCount=(u32)scissors.size(),
        .pScissors=scissors.data(),
    };
}

auto proc_raster_desc(
    RasterizationState rasterization_state
)->VkPipelineRasterizationStateCreateInfo{
    return VkPipelineRasterizationStateCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable=rasterization_state.depth_clamp_enable,
        .rasterizerDiscardEnable=rasterization_state.rasterizer_discard_enable,
        .polygonMode=(VkPolygonMode)rasterization_state.polygon_mode,
        .cullMode=(VkCullModeFlags)rasterization_state.cull_mode,
        .frontFace=(VkFrontFace)rasterization_state.front_face,
        .lineWidth=1.0f,
    };
}

auto proc_depthstencil_desc(
    DepthStencilState depth_stencil_state
)->VkPipelineDepthStencilStateCreateInfo{
    bool depth_enable=false;
    if(depth_stencil_state.depth.is_some()) depth_enable=true;
    auto depthstencil_desc=VkPipelineDepthStencilStateCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
    };
    if(depth_enable){
        depthstencil_desc.depthTestEnable=true;
        DepthState depth_state=depth_stencil_state.depth.unwrap();
        depthstencil_desc.depthWriteEnable=depth_state.write_enable;
        depthstencil_desc.depthCompareOp=(VkCompareOp)depth_state.compare_op;
    }
    return depthstencil_desc;
}

auto proc_dynamic_state_desc(
    vector<DynamicState>& dynamic_state
)->vector<VkDynamicState>{
    vector<VkDynamicState> s;
    for(auto state:dynamic_state){
        s.push_back(VkDynamicState(state));
    }
    return s;
}

// auto proc_rendering_desc(
//     RenderingCreateInfo render_pass
// )->VkPipelineRenderingCreateInfo{
//     return VkPipelineRenderingCreateInfo{
//         .sType=VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
//         .colorAttachmentCount=(u32)render_pass.color_attachment_formats.size(),
//         .pColorAttachmentFormats=(VkFormat*)render_pass.color_attachment_formats.data(),
//         .depthAttachmentFormat=(VkFormat)render_pass.depth_attachment_format,
//         .stencilAttachmentFormat=(VkFormat)render_pass.stencil_attachment_format
//     };
// }

auto proc_bindless_layout(u32 push_constant_size)->VkPipelineLayout{
    VkPipelineLayout pipeline_layout;
    VkPushConstantRange push_desc={
        .stageFlags=VK_SHADER_STAGE_ALL_GRAPHICS|VK_SHADER_STAGE_COMPUTE_BIT,
        .offset=0,
        .size=push_constant_size,
    };
    VkDescriptorSetLayout layouts[2]={
        VkDescriptorSetLayout(bindless_buffer_layout()),
        VkDescriptorSetLayout(bindless_image_layout())
    };
    auto layout_desc=VkPipelineLayoutCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount=2,
        .pSetLayouts=layouts,
        .pushConstantRangeCount=push_constant_size?1u:0,
        .pPushConstantRanges=push_constant_size?&push_desc:nullptr,
    };
    VK_CHECK(vkCreatePipelineLayout((VkDevice)device(),&layout_desc,nullptr,&pipeline_layout));
    return pipeline_layout;
}

auto GraphicsPipeline::build()->Result<GraphicsPipeline,Error>{
    
    VkPipelineShaderStageCreateInfo shader_desc[2]={
        proc_shader_info(vertex_shader_,VK_SHADER_STAGE_VERTEX_BIT),
        proc_shader_info(fragment_shader_,VK_SHADER_STAGE_FRAGMENT_BIT)
    };
    auto [bindings,attributes]=proc_vertex_input(vertex_input_state_);
    auto vertex_input_desc=proc_vertex_input_desc(bindings,attributes);
    auto input_assembly_desc=proc_input_assembly(input_assembly_state_);
    auto [viewports,scissors]=proc_viewport(viewport_state_);
    auto viewport_desc=proc_viewport_desc(viewports,scissors);
    auto raster_desc=proc_raster_desc(rasterization_state_);
    auto depthstencil_desc=proc_depthstencil_desc(depth_stencil_state_);
    // auto rendering_desc=proc_rendering_desc(render_pass_);
    auto pipeline_layout=proc_bindless_layout(push_constant_size_);

    auto multsamping_desc=VkPipelineMultisampleStateCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples=VK_SAMPLE_COUNT_1_BIT,
    };
    auto color_blend=VkPipelineColorBlendAttachmentState{
        .colorWriteMask=VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT,
    };
    auto color_blend_desc=VkPipelineColorBlendStateCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount=1,
        .pAttachments=&color_blend,
    };

    vector<VkFormat> color_formats;
    for(auto format:render_pass_.color_attachment_formats){
        color_formats.push_back((VkFormat)format);
    }
    auto rendering_desc=VkPipelineRenderingCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount=(u32)color_formats.size(),
        .pColorAttachmentFormats=(VkFormat*)color_formats.data(),
        .depthAttachmentFormat=(VkFormat)render_pass_.depth_attachment_format,
        .stencilAttachmentFormat=(VkFormat)render_pass_.stencil_attachment_format
    };

    auto dynamic_states=proc_dynamic_state_desc(dynamic_state_);
    auto dynamic_state_desc=VkPipelineDynamicStateCreateInfo{
        .sType=VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount=(u32)dynamic_states.size(),
        .pDynamicStates=dynamic_states.data(),
    };

    auto pipeline_desc=VkGraphicsPipelineCreateInfo{
        .sType=VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext=&rendering_desc,
        .stageCount=2,
        .pStages=shader_desc,
        .pVertexInputState=&vertex_input_desc,
        .pInputAssemblyState=&input_assembly_desc,
        .pViewportState=&viewport_desc,
        .pRasterizationState=&raster_desc,
        .pMultisampleState=&multsamping_desc,
        .pDepthStencilState=&depthstencil_desc,
        .pColorBlendState=&color_blend_desc,
        .pDynamicState=&dynamic_state_desc,
        .layout=pipeline_layout
    };
    VkPipeline pipeline;
    auto res=vkCreateGraphicsPipelines((VkDevice)device(),VK_NULL_HANDLE,1,&pipeline_desc,nullptr,&pipeline);
    if(res) return Err(Error(res));
    this->handle=(u64)pipeline;
    this->pipeline_layout=(u64)pipeline_layout;
    
    return Ok(*this);
}

}