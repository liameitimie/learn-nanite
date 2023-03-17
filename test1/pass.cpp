#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include "render_graph.h"
#include "resource.h"
#include "pass.h"
#include "vk_util.h"

using namespace std;

VkShaderModule load_shader(VkDevice device,string path){
    ifstream file(path,ios::ate|ios::binary);
    if(!file.is_open()){
        cout<<"failed to open file"<<endl;
        exit(1);
    }
    size_t code_size=(size_t)file.tellg();
    vector<char> code(code_size);
    file.seekg(0);
    file.read(code.data(),code_size);
    file.close();

    VkShaderModule shader;
    VkShaderModuleCreateInfo shader_desc={
        .sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize=code_size,
        .pCode=(uint32_t*)code.data(),
    };
    VK_CHECK(vkCreateShaderModule(device,&shader_desc,nullptr,&shader));
    return shader;
}


void create_graphic_pipeline(RenderGraph& rdg,Pass* pass,const json& pipeline_desc){
    VkShaderModule vs=load_shader(rdg.device(),pipeline_desc["vs"].get<string>());
    VkShaderModule ps=load_shader(rdg.device(),pipeline_desc["ps"].get<string>());
    VkPipelineShaderStageCreateInfo shader_stages[2]={
        VkPipelineShaderStageCreateInfo{
            .sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage=VK_SHADER_STAGE_VERTEX_BIT,
            .module=vs,
            .pName="main",
        },
        VkPipelineShaderStageCreateInfo{
            .sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage=VK_SHADER_STAGE_FRAGMENT_BIT,
            .module=ps,
            .pName="main",
        }
    };
    VkPipelineVertexInputStateCreateInfo vertex_input_desc={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };
    VkPipelineInputAssemblyStateCreateInfo input_assembly_desc={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };
    VkViewport viewport={
        .width=(float)rdg.width(),
        .height=(float)rdg.height(),
        .minDepth=0.0f,
        .maxDepth=1.0f,
    };
    VkRect2D scissor={
        .extent={
            .width=rdg.width(),
            .height=rdg.height(),
        }
    };
    VkPipelineViewportStateCreateInfo viewport_desc={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount=1,
        .pViewports=&viewport,
        .scissorCount=1,
        .pScissors=&scissor,
    };
    VkPipelineRasterizationStateCreateInfo rasterizer_desc={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .cullMode=VK_CULL_MODE_NONE,
        .lineWidth=1.0f,
    };
    VkPipelineMultisampleStateCreateInfo multsamping_desc={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples=VK_SAMPLE_COUNT_1_BIT,
    };
    VkPipelineColorBlendAttachmentState color_blend={
        .colorWriteMask=VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT,
    };
    VkPipelineColorBlendStateCreateInfo color_blend_desc={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount=1,
        .pAttachments=&color_blend,
    };
    VkPipelineLayout pipeline_layout;
    VkPipelineLayoutCreateInfo layout_desc={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };
    VK_CHECK(vkCreatePipelineLayout(rdg.device(),&layout_desc,nullptr,&pipeline_layout));
    
    VkPipelineRenderingCreateInfo rendering_info={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount=1,
        .pColorAttachmentFormats=&pass->attachment->format,
    };
    VkGraphicsPipelineCreateInfo pipeline_info={
        .sType=VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext=&rendering_info,
        .stageCount=2,
        .pStages=shader_stages,
        .pVertexInputState=&vertex_input_desc,
        .pInputAssemblyState=&input_assembly_desc,
        .pViewportState=&viewport_desc,
        .pRasterizationState=&rasterizer_desc,
        .pMultisampleState=&multsamping_desc,
        .pColorBlendState=&color_blend_desc,
        .layout=pipeline_layout,
    };
    VK_CHECK(vkCreateGraphicsPipelines(rdg.device(),VK_NULL_HANDLE,1,&pipeline_info,nullptr,&pass->pipeline));
}

Pass* Pass::create(RenderGraph& rdg,const json& pass_desc){
    Pass* pass=new Pass;

    string pipeline_type=pass_desc["pipeline"]["type"].get<string>();
    if(pipeline_type=="graphic"){
        pass->type=Graphic;
    }else if(pipeline_type=="compute"){
        pass->type=Compute;
    }else{
        cout<<"error pipeline type "<<pipeline_type<<endl;
        exit(0);
    }

    string attachment_name=pass_desc["attachment"].get<string>();
    pass->attachment=rdg.get_resource(attachment_name);

    if(pass->type==Graphic){
        create_graphic_pipeline(rdg,pass,pass_desc["pipeline"]);
    }
    else{
        cout<<"error: compute pipeline not support"<<endl;
        exit(0);
    }

    return pass;
}

void Pass::proc_pass(RenderGraph& rdg,VkCommandBuffer cmd_buffer){
    VkRenderingAttachmentInfo color_attachment_info={
        .sType=VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView=attachment->image_view,
        .imageLayout=VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
        .loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR,
    };
    VkRenderingInfo render_info={
        .sType=VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea={
            .extent{
                .width=rdg.width(),
                .height=rdg.height()
            }
        },
        .layerCount=1,
        .colorAttachmentCount=1,
        .pColorAttachments=&color_attachment_info,
    };
    vkCmdBeginRendering(cmd_buffer,&render_info);
    VkPipelineBindPoint bind_point;
    if(type==Graphic){
        bind_point=VK_PIPELINE_BIND_POINT_GRAPHICS;
    }else{

    }
    vkCmdBindPipeline(cmd_buffer,bind_point,pipeline);
    vkCmdDraw(cmd_buffer,3,1,0,0);
    vkCmdEndRendering(cmd_buffer);
}