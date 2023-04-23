#include <vulkan/vulkan.h>
#include "vk_context.h"
#include <vk_check.h>
#include <vector>
#include <result.h>
#include "error.h"
#include "image/image.h"
#include <iostream>
#include <vma/vk_mem_alloc.h>

using namespace std;

namespace vk{
namespace context{

    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue queue;
    u32 queue_family;

    VkDescriptorPool descriptor_pool;
    VkDescriptorSet bindless_buffer_set;
    VkDescriptorSet bindless_image_set;
    VkDescriptorSetLayout bindless_buffer_layout;
    VkDescriptorSetLayout bindless_image_layout;

    // VkSampler sampler;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkFormat swapchain_format;

    VmaAllocator alloctor;

    vector<Image> swapchain_images;
    u32 num_swapchain_images;

    // void create_default_sampler(){
    //     VkSamplerCreateInfo sampler_desc{
    //         .sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    //         .addressModeU=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    //         .addressModeV=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    //         .addressModeW=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    //         // .maxLod=VK_LOD_CLAMP_NONE,
    //         .unnormalizedCoordinates=VK_TRUE
    //     };
    //     VK_CHECK(vkCreateSampler(device,&sampler_desc,nullptr,&sampler));
    // }

    void create_bindless_buffer_layout(){
        VkDescriptorSetLayoutBinding binding={
            .binding=0,
            .descriptorType=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount=(1<<20),
            .stageFlags=VK_SHADER_STAGE_ALL
        };
        VkDescriptorBindingFlags flag=VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT
                                        | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
        VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flag={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .bindingCount=1,
            .pBindingFlags=&flag
        };
        VkDescriptorSetLayoutCreateInfo set_layout_desc={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext=&binding_flag,
            .bindingCount=1,
            .pBindings=&binding,
        };
        VK_CHECK(vkCreateDescriptorSetLayout(device,&set_layout_desc,nullptr,&bindless_buffer_layout));
    }

    void create_bindless_image_layout(){
        VkDescriptorSetLayoutBinding binding={
            .binding=0,
            .descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount=(1<<20),
            .stageFlags=VK_SHADER_STAGE_ALL
        };
        VkDescriptorBindingFlags flag=VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT
                                        | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
        VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flag={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .bindingCount=1,
            .pBindingFlags=&flag
        };
        VkDescriptorSetLayoutCreateInfo set_layout_desc={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext=&binding_flag,
            .bindingCount=1,
            .pBindings=&binding,
        };
        VK_CHECK(vkCreateDescriptorSetLayout(device,&set_layout_desc,nullptr,&bindless_image_layout));
    }

    void create_bindless_buffer_set(){
        VkDescriptorPoolSize pool_size={
            .type=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount=(1<<20)
        };
        VkDescriptorPoolCreateInfo desc_pool_info={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets=1,
            .poolSizeCount=1,
            .pPoolSizes=&pool_size,
        };
        VK_CHECK(vkCreateDescriptorPool(device,&desc_pool_info,nullptr,&descriptor_pool));

        u32 num=(1<<20);
        VkDescriptorSetVariableDescriptorCountAllocateInfo desc_set_ext={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
            .descriptorSetCount=1,
            .pDescriptorCounts=&num
        };
        VkDescriptorSetAllocateInfo desc_set_info={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext=&desc_set_ext,
            .descriptorPool=descriptor_pool,
            .descriptorSetCount=1,
            .pSetLayouts=&bindless_buffer_layout
        };
        VK_CHECK(vkAllocateDescriptorSets(device,&desc_set_info,&bindless_buffer_set));
    }

    void create_bindless_image_set(){
        VkDescriptorPoolSize pool_size={
            .type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount=(1<<20)
        };
        VkDescriptorPoolCreateInfo desc_pool_info={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets=1,
            .poolSizeCount=1,
            .pPoolSizes=&pool_size,
        };
        VK_CHECK(vkCreateDescriptorPool(device,&desc_pool_info,nullptr,&descriptor_pool));

        u32 num=(1<<20);
        VkDescriptorSetVariableDescriptorCountAllocateInfo desc_set_ext={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
            .descriptorSetCount=1,
            .pDescriptorCounts=&num
        };
        VkDescriptorSetAllocateInfo desc_set_info={
            .sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext=&desc_set_ext,
            .descriptorPool=descriptor_pool,
            .descriptorSetCount=1,
            .pSetLayouts=&bindless_image_layout
        };
        VK_CHECK(vkAllocateDescriptorSets(device,&desc_set_info,&bindless_image_set));
    }

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
            VkPhysicalDeviceDescriptorIndexingFeatures indexing_feature={
                .sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
                .pNext=&sync2_feature,
                .shaderStorageBufferArrayNonUniformIndexing=VK_TRUE,
                .descriptorBindingPartiallyBound=VK_TRUE,
                .descriptorBindingVariableDescriptorCount=VK_TRUE,
                .runtimeDescriptorArray=VK_TRUE,
            };

            VkDeviceCreateInfo dev_desc={
                .sType=VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext=&indexing_feature,
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
        }
        {
            VmaAllocatorCreateInfo allocator_desc={
                .physicalDevice=physical_device,
                .device=device,
                .instance=instance,
            };
            VK_CHECK(vmaCreateAllocator(&allocator_desc,&alloctor));
        }
        {
            create_bindless_buffer_layout();
            create_bindless_image_layout();

            create_bindless_buffer_set();
            create_bindless_image_set();

            // create_default_sampler();
        }
        // {
        //     VkPhysicalDeviceSubgroupProperties pro1={
        //         .sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES,
        //     };
        //     VkPhysicalDeviceProperties2 pro={
        //         .sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        //         .pNext=&pro1
        //     };
        //     vkGetPhysicalDeviceProperties2(physical_device,&pro);
        //     vkGetPhysicalDeviceProperties2(physical_device,&pro);
        // }
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
                .imageUsage=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT,
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
            num_swapchain_images=swapchain_image_cnt;
            for(int i=0;i<swapchain_image_cnt;i++){
                swapchain_images[i]={};
                swapchain_images[i].handle=(u64)vk_swapchain_images[i];
                swapchain_images[i].format=(Format)swapchain_format;
                swapchain_images[i].dimensions=ImageDimensions{
                    .type=ImageDimensions::Dim2d,
                    .width=width,
                    .height=height
                };
                swapchain_images[i].usage=ImageUsage::ColorAttachment;
                swapchain_images[i].mip_levels=1;
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
u32 num_swapchain_image(){return context::num_swapchain_images;/*context::swapchain_images.size();*/}


u64 bindless_buffer_layout(){return (u64)context::bindless_buffer_layout;}
u64 bindless_buffer_set(){return (u64)context::bindless_buffer_set;}

u64 bindless_image_layout(){return (u64)context::bindless_image_layout;}
u64 bindless_image_set(){return (u64)context::bindless_image_set;}

// u64 default_sampler(){return (u64)context::sampler;}

void cleanup(){
    vkDeviceWaitIdle((VkDevice)device());
}

}// namespace vk