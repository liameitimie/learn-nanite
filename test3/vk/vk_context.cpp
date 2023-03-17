#include <vulkan/vulkan.h>
#include "vk_context.h"
#include <vk_check.h>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <result.h>
#include "error.h"
#include "image/image.h"
#include <iostream>
#include <vma/vk_mem_alloc.h>

using namespace std;

namespace vk{
namespace context{

    static VkInstance instance;
    static VkPhysicalDevice physical_device;
    static VkDevice device;
    static VkQueue queue;
    static u32 queue_family;
    static VkSurfaceKHR surface;
    static VkSwapchainKHR swapchain;
    static VkFormat swapchain_format;

    static VmaAllocator alloctor;

    static vector<Image> swapchain_images;

    void init(){
        u32 ins_ext_cnt=2;
        const char* ins_exts[]={"VK_KHR_surface","VK_KHR_win32_surface"};
        u32 layer_cnt=1;
        const char* validation_layers[]={"VK_LAYER_KHRONOS_validation"};
        u32 dev_ext_cnt=1;
        const char* dev_exts[]={"VK_KHR_swapchain"};

        {   // create instance
            VkApplicationInfo app_info={
                .sType=VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .apiVersion=VK_API_VERSION_1_3,
            };

            VkInstanceCreateInfo instance_desc={
                .sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pApplicationInfo=&app_info,
                .enabledLayerCount=layer_cnt,
                .ppEnabledLayerNames=validation_layers,
                .enabledExtensionCount=ins_ext_cnt,
                .ppEnabledExtensionNames=ins_exts,
            };
            VK_CHECK(vkCreateInstance(&instance_desc,nullptr,&instance));
        }
        {   // get physical device
            uint32_t physical_device_cnt=0;
            vkEnumeratePhysicalDevices(instance,&physical_device_cnt,nullptr);
            if(physical_device_cnt==0){
                cout<<"failed to find GPUs with Vulkan support"<<endl;
                exit(0);
            }
            vector<VkPhysicalDevice> physical_devices(physical_device_cnt);
            vkEnumeratePhysicalDevices(instance,&physical_device_cnt,physical_devices.data());
            
            physical_device=physical_devices[0];
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physical_device,&properties);
            cout<<"using: "<<properties.deviceName<<endl;
        }
        {
            {
                u32 qf_cnt=0;
                vkGetPhysicalDeviceQueueFamilyProperties(physical_device,&qf_cnt,nullptr);
                vector<VkQueueFamilyProperties> qfs(qf_cnt);
                vkGetPhysicalDeviceQueueFamilyProperties(physical_device,&qf_cnt,qfs.data());
                u32 i=0;
                for(auto& qf:qfs){
                    if(qf.queueFlags&VK_QUEUE_GRAPHICS_BIT){
                        queue_family=i;break;
                    }
                    i++;
                }
            }
            float q_priority=1.f;
            VkDeviceQueueCreateInfo queue_desc={
                .sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex=queue_family,
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

            VK_CHECK(vkCreateDevice(physical_device,&dev_desc,nullptr,&device));
            vkGetDeviceQueue(device,queue_family,0,&queue);

            VmaAllocatorCreateInfo allocator_desc={
                .physicalDevice=physical_device,
                .device=device,
                .instance=instance,
            };
            vmaCreateAllocator(&allocator_desc,&alloctor);
        }
    }

    void init_surface(u64 handle,u32 width,u32 height){
        surface=(VkSurfaceKHR)handle;
        {
            swapchain_format=VK_FORMAT_B8G8R8A8_UNORM;
            VkSwapchainCreateInfoKHR swapchain_desc={
                .sType=VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface=surface,
                .minImageCount=3,
                .imageFormat=swapchain_format,
                .imageExtent={width,height},
                .imageArrayLayers=1,
                .imageUsage=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .preTransform=VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
                .compositeAlpha=VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode=VK_PRESENT_MODE_MAILBOX_KHR
            };
            VK_CHECK(vkCreateSwapchainKHR(device,&swapchain_desc,nullptr,&swapchain));

            uint32_t swapchain_image_cnt=0;
            VK_CHECK(vkGetSwapchainImagesKHR(device,swapchain,&swapchain_image_cnt,nullptr));
            vector<VkImage> vk_swapchain_images(swapchain_image_cnt);
            VK_CHECK(vkGetSwapchainImagesKHR(device,swapchain,&swapchain_image_cnt,vk_swapchain_images.data()));

            swapchain_images.resize(swapchain_image_cnt);
            for(int i=0;i<swapchain_image_cnt;i++){
                swapchain_images[i]={};
                swapchain_images[i].handle=(u64)vk_swapchain_images[i];
                swapchain_images[i].format=(Format)swapchain_format;
                swapchain_images[i].dimensions=ImageDimensions{
                    .type=ImageDimensions::Dim2d,
                    .width=width,
                    .height=height
                };
                VkImageViewCreateInfo imgv_desc={
                    .sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image=vk_swapchain_images[i],
                    .viewType=VK_IMAGE_VIEW_TYPE_2D,
                    .format=swapchain_format,
                    .subresourceRange={
                        .aspectMask=VK_IMAGE_ASPECT_COLOR_BIT,
                        .levelCount=1,
                        .layerCount=1
                    }
                };
                VK_CHECK(vkCreateImageView(device,&imgv_desc,nullptr,(VkImageView*)&swapchain_images[i].image_view));
            }
        }
    }
}// namespace context

void init(){context::init();}
void init_surface(u64 handle,u32 width,u32 height){context::init_surface(handle,width,height);}

u64 instance(){return (u64)context::instance;}
u64 physical_device(){return (u64)context::physical_device;}
u64 device(){return (u64)context::device;}
u64 queue(){return (u64)context::queue;}
u32 queue_family(){return context::queue_family;}
u64 swapchain(){return (u64)context::swapchain;}

u64 allocator(){return (u64)context::alloctor;}

Image swapchain_image(u32 idx){return context::swapchain_images[idx];}
Format swapchain_image_format(){return context::swapchain_images[0].format;}
u32 num_swapchain_image(){return context::swapchain_images.size();}

void cleanup(){
    vkDeviceWaitIdle((VkDevice)device());
}

}// namespace vk