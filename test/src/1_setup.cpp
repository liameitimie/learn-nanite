#include "vk.h"
#include "enum_device.h"
#include "vk_check.h"
#include <fstream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <stdint.h>
#include <vcruntime.h>
#include <vector>
#include <algorithm>
#include <chrono>

VkInstance ins;
VkPhysicalDevice adp;
VkDevice dev;

const uint32_t width=800,height=600;

void get_swapchain_image(VkDevice dev,VkSwapchainKHR swapchain,std::vector<VkImage>& images){
    uint32_t image_cnt=0;
    vkGetSwapchainImagesKHR(dev,swapchain,&image_cnt,nullptr);
    images.resize(image_cnt);
    vkGetSwapchainImagesKHR(dev,swapchain,&image_cnt,images.data());
}

VkImageView create_image_view(VkDevice dev,VkImage img,VkFormat format){
    VkImageViewCreateInfo imgv_desc{};
    imgv_desc.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgv_desc.image=img;
    imgv_desc.viewType=VK_IMAGE_VIEW_TYPE_2D;
    imgv_desc.format=format;
    imgv_desc.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
    imgv_desc.subresourceRange.levelCount=1;
    imgv_desc.subresourceRange.layerCount=1;

    VkImageView img_view;
    VK_CHECK(vkCreateImageView(dev,&imgv_desc,nullptr,&img_view));
    return img_view;
}

VkShaderModule load_shader(std::string path){
    std::ifstream file(path,std::ios::ate|std::ios::binary);
    if(!file.is_open()){
        std::cout<<"failed to open file"<<std::endl;
        exit(1);
    }
    size_t code_size=(size_t)file.tellg();
    std::vector<char> code(code_size);
    file.seekg(0);
    file.read(code.data(),code_size);
    file.close();

    VkShaderModule shader;
    VkShaderModuleCreateInfo shader_desc={
        .sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize=code_size,
        .pCode=(uint32_t*)code.data(),
    };
    VK_CHECK(vkCreateShaderModule(dev,&shader_desc,nullptr,&shader));
    return shader;
}

using namespace std::chrono;
class TimerClock{
public:
    TimerClock(){
        update();
    }
    void update(){
        _start = high_resolution_clock::now();
    }
    long long getTimerMicroSec(){
    //当前时钟减去开始时钟的count
        return duration_cast<microseconds>(high_resolution_clock::now() - _start).count();
    }
private:
    time_point<high_resolution_clock>_start;
};

int main(){
    // enum_instance_extension();
    // enum_instance_layer();

    init_glfw();
    ins=create_instance(true);
    adp=get_adapter(0);
    dev=create_device(adp);
    VkQueue que=get_queue(dev);
    
    GLFWwindow* window=glfwCreateWindow(width,height,"learn vulkan",nullptr,nullptr);
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(ins,window,nullptr,&surface);
    // enum_device_extension(adp);
    // enum_physical_device(ins);
    // query_surface_capabilities(adp,surface);

    //VkFormat swapchain_format=VK_FORMAT_B8G8R8A8_SRGB;
    VkFormat swapchain_format=VK_FORMAT_B8G8R8A8_UNORM;
    //VkFormat swapchain_format=VK_FORMAT_A2B10G10R10_UNORM_PACK32;

    VkSwapchainKHR swapchain=create_swapchain(dev,surface,width,height,swapchain_format);
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    get_swapchain_image(dev,swapchain,swapchain_images);
    swapchain_image_views.resize(swapchain_images.size());
    for(int i=0;i<swapchain_images.size();i++){
        auto& imgv=swapchain_image_views[i];
        imgv=create_image_view(dev,swapchain_images[i],swapchain_format);
    }

    //create pipeline

    VkShaderModule vert_shader=load_shader("hello_triangle_vert.spv");
    VkShaderModule frag_shader=load_shader("hello_triangle_frag.spv");

    VkPipelineShaderStageCreateInfo shader_stages[2];
    shader_stages[0]={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage=VK_SHADER_STAGE_VERTEX_BIT,
        .module=vert_shader,
        .pName="main",
    };
    shader_stages[1]={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage=VK_SHADER_STAGE_FRAGMENT_BIT,
        .module=frag_shader,
        .pName="main",
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_desc={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };
    VkPipelineInputAssemblyStateCreateInfo input_assembly_desc={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };
    VkViewport viewport={
        .width=width,
        .height=height,
        .minDepth=0.0f,
        .maxDepth=1.0f,
    };
    VkRect2D scissor={
        .extent={
            .width=width,
            .height=height,
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
    VK_CHECK(vkCreatePipelineLayout(dev,&layout_desc,nullptr,&pipeline_layout));

    VkPipelineRenderingCreateInfo pipeline_desc_ext={
        .sType=VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount=1,
        .pColorAttachmentFormats=&swapchain_format,
    };
    VkGraphicsPipelineCreateInfo pipeline_desc={
        .sType=VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext=&pipeline_desc_ext,
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

    VkPipeline pipeline;
    VK_CHECK(vkCreateGraphicsPipelines(dev,VK_NULL_HANDLE,1,&pipeline_desc,nullptr,&pipeline));

    VkCommandPool cmd_pool;
    VkCommandPoolCreateInfo cmd_pool_desc={
        .sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex=get_queue_index(),
    };
    VK_CHECK(vkCreateCommandPool(dev,&cmd_pool_desc,nullptr,&cmd_pool));

    std::vector<VkCommandBuffer> cmd_buffers(swapchain_images.size());
    VkCommandBufferAllocateInfo cmd_buffer_alloc_desc={
        .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool=cmd_pool,
        .level=VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount=(uint32_t)cmd_buffers.size(),
    };
    VK_CHECK(vkAllocateCommandBuffers(dev,&cmd_buffer_alloc_desc,cmd_buffers.data()));

    for(int i=0;i<cmd_buffers.size();i++){
        VkCommandBuffer& cmd_buffer=cmd_buffers[i];

        VkCommandBufferBeginInfo begin_info={
            .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags=VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        };
        VK_CHECK(vkBeginCommandBuffer(cmd_buffer,&begin_info));

        VkRenderingAttachmentInfo color_attachment_info={
            .sType=VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView=swapchain_image_views[i],
            .imageLayout=VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR,
        };
        VkRenderingInfo render_info={
            .sType=VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea=scissor,
            .layerCount=1,
            .colorAttachmentCount=1,
            .pColorAttachments=&color_attachment_info,
        };

        VkImageMemoryBarrier2 barrier1={
            .sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .dstStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask=VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout=VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .image=swapchain_images[i],
            .subresourceRange={
                .aspectMask=VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount=1,
                .layerCount=1,
            }
        };
        VkDependencyInfo dependency1={
            .sType=VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount=1,
            .pImageMemoryBarriers=&barrier1
        };
        vkCmdPipelineBarrier2(cmd_buffer,&dependency1);

        vkCmdBeginRendering(cmd_buffer,&render_info);
        vkCmdBindPipeline(cmd_buffer,VK_PIPELINE_BIND_POINT_GRAPHICS,pipeline);
        vkCmdDraw(cmd_buffer,3,1,0,0);
        vkCmdEndRendering(cmd_buffer);

        VkImageMemoryBarrier2 barrier2={
            .sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask=VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout=VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image=swapchain_images[i],
            .subresourceRange={
                .aspectMask=VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount=1,
                .layerCount=1,
            }
        };
        VkDependencyInfo dependency2={
            .sType=VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount=1,
            .pImageMemoryBarriers=&barrier2
        };
        vkCmdPipelineBarrier2(cmd_buffer,&dependency2);

        VK_CHECK(vkEndCommandBuffer(cmd_buffer));
    }

    VkSemaphore semaphores[6];
    VkFence fences[3];
    VkSemaphoreCreateInfo semaphore_desc={
        .sType=VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };
    VkFenceCreateInfo fence_desc={
        .sType=VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags=VK_FENCE_CREATE_SIGNALED_BIT
    };
    for(int i=0;i<6;i++){
        VK_CHECK(vkCreateSemaphore(dev,&semaphore_desc,nullptr,&semaphores[i]));
    }
    for(int i=0;i<3;i++){
        VK_CHECK(vkCreateFence(dev,&fence_desc,nullptr,&fences[i]));
    }

    TimerClock timer;
    uint64_t sum_time=0,frame_cnt=0;
    uint64_t min_times[3]={(uint64_t)(-1),(uint64_t)(-1),(uint64_t)(-1)};
    uint64_t max_times[3]={0,0,0};

    int current_frame=0;
    while(!glfwWindowShouldClose(window)){
        timer.update();

        glfwPollEvents();

        uint32_t image_index;
        vkAcquireNextImageKHR(dev,swapchain,(uint64_t)(-1),semaphores[current_frame*2],nullptr,&image_index);
        vkWaitForFences(dev,1,&fences[current_frame],VK_TRUE,(uint64_t)(-1));
        vkResetFences(dev,1,&fences[current_frame]);
        
        VkPipelineStageFlags wait_stages=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submit_info={
            .sType=VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount=1,
            .pWaitSemaphores=&semaphores[current_frame*2],
            .pWaitDstStageMask=&wait_stages,
            .commandBufferCount=1,
            .pCommandBuffers=&cmd_buffers[image_index],
            .signalSemaphoreCount=1,
            .pSignalSemaphores=&semaphores[current_frame*2+1],
        };
        VK_CHECK(vkQueueSubmit(que,1,&submit_info,fences[current_frame]));

        VkPresentInfoKHR present_info={
            .sType=VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount=1,
            .pWaitSemaphores=&semaphores[current_frame*2+1],
            .swapchainCount=1,
            .pSwapchains=&swapchain,
            .pImageIndices=&image_index,
        };
        vkQueuePresentKHR(que,&present_info);

        current_frame=(current_frame+1)%3;

        uint64_t frame_time=timer.getTimerMicroSec();
        // std::cout<<frame_time/1000.f<<"\n";
        sum_time+=frame_time;
        frame_cnt++;

        if(frame_time<min_times[2]){
            min_times[2]=frame_time;
            if(min_times[2]<min_times[1]){
                std::swap(min_times[1],min_times[2]);
                if(min_times[1]<min_times[0]) std::swap(min_times[0],min_times[1]);
            }
        }

        if(frame_time>max_times[2]){
            max_times[2]=frame_time;
            if(max_times[2]>max_times[1]){
                std::swap(max_times[1],max_times[2]);
                if(max_times[1]>max_times[0]) std::swap(max_times[0],max_times[1]);
            }
        }
    }
    vkDeviceWaitIdle(dev);

    std::cout<<"frame cnt:"<<frame_cnt<<", averge frame time:"<<sum_time/1000.f/frame_cnt<<std::endl;
    std::cout<<"min times:"<<min_times[0]/1000.f<<' '<<min_times[1]/1000.f<<' '<<min_times[2]/1000.f<<std::endl;
    std::cout<<"max times:"<<max_times[0]/1000.f<<' '<<max_times[1]/1000.f<<' '<<max_times[2]/1000.f<<std::endl;

    return 0;
}