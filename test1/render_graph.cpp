#include "render_graph.h"
#include "GLFW/glfw3.h"
#include "event.h"
#include "pass.h"
#include "resource.h"
#include <stdint.h>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

const uint32_t width=1920,height=1080;

struct VkContext{
    VkInstance instance;
    vector<VkPhysicalDevice> physical_devices;
    VkDevice device;
    uint32_t queue_family_idx;
    VkQueue queue;
    GLFWwindow* window;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkFormat swapchain_format;
    vector<VkImage> swapchain_images;
    vector<VkImageView> swapchain_image_views;
};

struct RenderGraphImpl{
    VkContext context;
    unordered_map<string,Resource*> resources;
    unordered_map<string,Pass*> passes;
    vector<Event*> events;

    vector<VkCommandBuffer> command_buffers;

    //sync frame
    VkSemaphore semaphores[6];
    VkFence fences[3];
};

void RenderGraph::from_json_file(string path){
    ifstream in(path);
    json desc=json::parse(in);
    from_json(desc);
}

void RenderGraph::from_json(const json& desc){
    for(auto& pass_desc:desc["passes"]){
        add_pass(pass_desc);
    }
    for(auto& event_desc:desc["events"]){
        add_event(event_desc);
    }
    build();
}

void RenderGraph::add_event(const json& event_desc){
    impl->events.push_back(Event::create(*this,event_desc));
}

void RenderGraph::add_pass(const json& pass_desc){
    string name=pass_desc["name"].get<string>();
    impl->passes.insert({name,Pass::create(*this,pass_desc)});
}

void build_cmd_buffer(RenderGraph& rdg,VkCommandBuffer cmd_buffer){
    Resource* __output=rdg.impl->resources["__output"];
    VkCommandBufferBeginInfo cmd_begin_info={
        .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
       .flags=VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };
    VK_CHECK(vkBeginCommandBuffer(cmd_buffer,&cmd_begin_info));

    VkImageMemoryBarrier2 barrier1={
        .sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .dstStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask=VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout=VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .image=__output->image,
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

    for(auto event:rdg.events()){
        event->proc_event(rdg,cmd_buffer);
    }

    VkImageMemoryBarrier2 barrier2={
        .sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask=VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout=VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .image=__output->image,
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

void RenderGraph::build(){
    VkCommandPool cmd_pool;
    VkCommandPoolCreateInfo cmd_pool_desc={
        .sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex=impl->context.queue_family_idx,
    };
    VK_CHECK(vkCreateCommandPool(device(),&cmd_pool_desc,nullptr,&cmd_pool));

    impl->command_buffers.resize(impl->context.swapchain_images.size());
    VkCommandBufferAllocateInfo cmd_buffer_alloc_desc={
        .sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool=cmd_pool,
        .level=VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount=(uint32_t)impl->command_buffers.size(),
    };
    VK_CHECK(vkAllocateCommandBuffers(device(),&cmd_buffer_alloc_desc,impl->command_buffers.data()));

    for(int i=0;i<impl->command_buffers.size();i++){
        Resource* __output=impl->resources["__output"];
        __output->image=impl->context.swapchain_images[i];
        __output->image_view=impl->context.swapchain_image_views[i];

        build_cmd_buffer(*this,impl->command_buffers[i]);
    }
}

void RenderGraph::render(){
    static uint32_t frame_idx=0;
    frame_idx=(frame_idx+1)%3;

    uint32_t swapchain_image_idx;
    vkAcquireNextImageKHR(device(),impl->context.swapchain,(uint64_t)(-1),impl->semaphores[frame_idx*2],nullptr,&swapchain_image_idx);
    vkWaitForFences(device(),1,&impl->fences[frame_idx],VK_TRUE,(uint64_t)(-1));
    vkResetFences(device(),1,&impl->fences[frame_idx]);

    VkPipelineStageFlags wait_stages=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info={
        .sType=VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount=1,
        .pWaitSemaphores=&impl->semaphores[frame_idx*2],
        .pWaitDstStageMask=&wait_stages,
        .commandBufferCount=1,
        .pCommandBuffers=&impl->command_buffers[swapchain_image_idx],
        .signalSemaphoreCount=1,
        .pSignalSemaphores=&impl->semaphores[frame_idx*2+1],
    };
    VK_CHECK(vkQueueSubmit(impl->context.queue,1,&submit_info,impl->fences[frame_idx]));

    VkPresentInfoKHR present_info={
        .sType=VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount=1,
        .pWaitSemaphores=&impl->semaphores[frame_idx*2+1],
        .swapchainCount=1,
        .pSwapchains=&impl->context.swapchain,
        .pImageIndices=&swapchain_image_idx,
    };
    vkQueuePresentKHR(impl->context.queue,&present_info);
}

void RenderGraph::clear(){
    vkDeviceWaitIdle(device());
}

VkDevice RenderGraph::device(){
    return impl->context.device;
}
GLFWwindow* RenderGraph::window(){
    return impl->context.window;
}
uint32_t RenderGraph::width(){
    return ::width;
}
uint32_t RenderGraph::height(){
    return ::height;
}

Resource* RenderGraph::get_resource(string name){
    return impl->resources[name];
}
Pass* RenderGraph::get_pass(string name){
    return impl->passes[name];
}
const vector<Event*>& RenderGraph::events(){
    return impl->events;
}

void RenderGraph::init(){
    impl=new RenderGraphImpl;
    VkContext& context=impl->context;

    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

    uint32_t layer_cnt=1;
    const char* validation_layers[]={"VK_LAYER_KHRONOS_validation"};
    uint32_t dev_ext_cnt=1;
    const char* dev_exts[]={"VK_KHR_swapchain"};

    {   // create instance
        VkApplicationInfo app_info={
            .sType=VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .apiVersion=VK_API_VERSION_1_3,
        };
        uint32_t glfw_ext_cnt=0;
        const char** glfw_exts;
        glfw_exts=glfwGetRequiredInstanceExtensions(&glfw_ext_cnt);

        VkInstanceCreateInfo instance_desc={
            .sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo=&app_info,
            .enabledLayerCount=layer_cnt,
            .ppEnabledLayerNames=validation_layers,
            .enabledExtensionCount=glfw_ext_cnt,
            .ppEnabledExtensionNames=glfw_exts,
        };
        VK_CHECK(vkCreateInstance(&instance_desc,nullptr,&context.instance));
    }
    {   // get physical device
        uint32_t physical_device_cnt=0;
        vkEnumeratePhysicalDevices(context.instance,&physical_device_cnt,nullptr);
        if(physical_device_cnt==0){
            cout<<"failed to find GPUs with Vulkan support"<<endl;
            exit(0);
        }
        context.physical_devices.resize(physical_device_cnt);
        vkEnumeratePhysicalDevices(context.instance,&physical_device_cnt,context.physical_devices.data());

        cout<<physical_device_cnt<<endl;
    }
    {   //create device
        VkPhysicalDevice physical_device=context.physical_devices[0];
        {
            uint32_t qf_cnt=0;
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device,&qf_cnt,nullptr);
            vector<VkQueueFamilyProperties> qfs(qf_cnt);
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device,&qf_cnt,qfs.data());
            int i=0;
            for(auto& qf:qfs){
                if(qf.queueFlags&VK_QUEUE_GRAPHICS_BIT){
                    context.queue_family_idx=i;break;
                }
                i++;
            }
        }
        float q_priority=1.f;
        VkDeviceQueueCreateInfo queue_desc={
            .sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex=context.queue_family_idx,
            .queueCount=1,
            .pQueuePriorities=&q_priority
        };
        VkPhysicalDeviceFeatures dev_feature{};
        VkPhysicalDeviceDynamicRenderingFeatures dyn_render_feature={
            .sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .dynamicRendering=VK_TRUE
        };
        VkPhysicalDeviceSynchronization2Features sync2_feature={
            .sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext=&dyn_render_feature,
            .synchronization2=VK_TRUE
        };

        VkDeviceCreateInfo dev_desc={
            .sType=VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext=&sync2_feature,
            .queueCreateInfoCount=1,
            .pQueueCreateInfos=&queue_desc,
            .enabledLayerCount=layer_cnt,
            .ppEnabledLayerNames=validation_layers,
            .enabledExtensionCount=dev_ext_cnt,
            .ppEnabledExtensionNames=dev_exts,
            .pEnabledFeatures=&dev_feature
        };

        VK_CHECK(vkCreateDevice(physical_device,&dev_desc,nullptr,&context.device));
    }
    {   // get queue
        vkGetDeviceQueue(context.device,context.queue_family_idx,0,&context.queue);
    }
    {   //create window, surface, swapchain
        context.window=glfwCreateWindow(::width,::height,"learn nanite",nullptr,nullptr);
        VK_CHECK(glfwCreateWindowSurface(context.instance,context.window,nullptr,&context.surface));
        context.swapchain_format=VK_FORMAT_B8G8R8A8_UNORM;
        VkSwapchainCreateInfoKHR swapchain_desc={
            .sType=VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface=context.surface,
            .minImageCount=3,
            .imageFormat=context.swapchain_format,
            .imageExtent={::width,::height},
            .imageArrayLayers=1,
            .imageUsage=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform=VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha=VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode=VK_PRESENT_MODE_MAILBOX_KHR
        };
        VK_CHECK(vkCreateSwapchainKHR(context.device,&swapchain_desc,nullptr,&context.swapchain));

        uint32_t swapchain_image_cnt=0;
        VK_CHECK(vkGetSwapchainImagesKHR(context.device,context.swapchain,&swapchain_image_cnt,nullptr));
        context.swapchain_images.resize(swapchain_image_cnt);
        VK_CHECK(vkGetSwapchainImagesKHR(context.device,context.swapchain,&swapchain_image_cnt,context.swapchain_images.data()));

        for(VkImage img:context.swapchain_images){
            VkImageView imgv;
            VkImageViewCreateInfo imgv_desc={
                .sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image=img,
                .viewType=VK_IMAGE_VIEW_TYPE_2D,
                .format=context.swapchain_format,
                .subresourceRange={
                    .aspectMask=VK_IMAGE_ASPECT_COLOR_BIT,
                    .levelCount=1,
                    .layerCount=1
                }
            };
            VK_CHECK(vkCreateImageView(context.device,&imgv_desc,nullptr,&imgv));
            context.swapchain_image_views.push_back(imgv);
        }
    }
    {   // default resource
        Resource* __output=new Resource;
        __output->type=Resource::Image;
        __output->format=context.swapchain_format;
        impl->resources.insert({"__output",__output});
    }
    {
        VkSemaphoreCreateInfo semaphore_desc={
            .sType=VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        VkFenceCreateInfo fence_desc={
            .sType=VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags=VK_FENCE_CREATE_SIGNALED_BIT
        };
        for(int i=0;i<6;i++){
            VK_CHECK(vkCreateSemaphore(device(),&semaphore_desc,nullptr,&impl->semaphores[i]));
        }
        for(int i=0;i<3;i++){
            VK_CHECK(vkCreateFence(device(),&fence_desc,nullptr,&impl->fences[i]));
        }
    }
}
