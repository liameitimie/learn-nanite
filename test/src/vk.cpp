#include "vk.h"
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <string.h>
#include <vector>

std::vector<const char*> validation_layers={
    "VK_LAYER_KHRONOS_validation"
};

std::vector<const char*> device_ext={
    "VK_KHR_swapchain"
};

bool enable_validation;
std::vector<VkPhysicalDevice> physical_devices;
uint32_t queue_family_idx;

void init_glfw(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
}

bool check_validation_layer_support(){
    uint32_t layer_cnt=0;
    vkEnumerateInstanceLayerProperties(&layer_cnt,nullptr);
    std::vector<VkLayerProperties> layers(layer_cnt);
    vkEnumerateInstanceLayerProperties(&layer_cnt,layers.data());

    for(const char* layer_name:validation_layers){
        bool layer_found=false;
        for(auto& layer:layers){
            if(strcmp(layer_name,layer.layerName)==0){
                layer_found=true;break;
            }
        }
        if(!layer_found) return false;
    }
    return true;
}

VkInstance create_instance(bool _enable_validation){
    enable_validation=_enable_validation;
    VkInstance ins;

    VkApplicationInfo app_desc{};
    app_desc.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_desc.apiVersion=VK_API_VERSION_1_3;

    std::vector<const char*> req_exts;
    {//get required extensions
        uint32_t glfw_ext_cnt=0;
        const char** glfw_exts;
        glfw_exts=glfwGetRequiredInstanceExtensions(&glfw_ext_cnt);
        req_exts=std::vector<const char*>(glfw_exts,glfw_exts+glfw_ext_cnt);

        if(enable_validation){
            req_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
    }
    VkInstanceCreateInfo ins_desc{};
    ins_desc.sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ins_desc.pApplicationInfo=&app_desc;

    ins_desc.enabledExtensionCount=req_exts.size();
    ins_desc.ppEnabledExtensionNames=req_exts.data();

    if(enable_validation){
        if(!check_validation_layer_support()){
            std::cout<<"warning: validation layer is not support"<<std::endl;
        }
        else{
            ins_desc.enabledLayerCount=validation_layers.size();
            ins_desc.ppEnabledLayerNames=validation_layers.data();
        }
    }

    auto vkerr=vkCreateInstance(&ins_desc,nullptr,&ins);
    if(vkerr!=VK_SUCCESS){
        throw std::runtime_error("failed to create VkInstance");
    }

    uint32_t dev_cnt=0;
    vkEnumeratePhysicalDevices(ins,&dev_cnt,nullptr);
    if(dev_cnt==0){
        throw std::runtime_error("failed to find GPUs with Vulkan support");
    }
    physical_devices.resize(dev_cnt);
    vkEnumeratePhysicalDevices(ins,&dev_cnt,physical_devices.data());
    
    return ins;
}

VkPhysicalDevice get_adapter(int idx){
    return physical_devices[idx];
}

VkDevice create_device(VkPhysicalDevice adapter){
    VkDevice dev;

    uint32_t qf_cnt=0;
    vkGetPhysicalDeviceQueueFamilyProperties(adapter,&qf_cnt,nullptr);
    std::vector<VkQueueFamilyProperties> qfs(qf_cnt);
    vkGetPhysicalDeviceQueueFamilyProperties(adapter,&qf_cnt,qfs.data());
    int i=0;
    for(auto& qf:qfs){
        if(qf.queueFlags&VK_QUEUE_GRAPHICS_BIT){
            queue_family_idx=i;break;
        }
        i++;
    }

    float q_priority=1.f;
    VkDeviceQueueCreateInfo queue_desc{};
    queue_desc.sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_desc.queueFamilyIndex=queue_family_idx;
    queue_desc.queueCount=1;
    queue_desc.pQueuePriorities=&q_priority;

    VkPhysicalDeviceFeatures dev_feature{};
    VkPhysicalDeviceDynamicRenderingFeatures dev_feature_ext1{};
    dev_feature_ext1.sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dev_feature_ext1.dynamicRendering=VK_TRUE;
    VkPhysicalDeviceSynchronization2Features dev_feature_ext{};
    dev_feature_ext.sType=VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    dev_feature_ext.synchronization2=VK_TRUE;
    dev_feature_ext.pNext=&dev_feature_ext1;

    VkDeviceCreateInfo dev_desc{};
    dev_desc.sType=VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dev_desc.pNext=&dev_feature_ext;
    dev_desc.pQueueCreateInfos=&queue_desc;
    dev_desc.queueCreateInfoCount=1;
    dev_desc.pEnabledFeatures=&dev_feature;
    dev_desc.enabledExtensionCount=device_ext.size();
    dev_desc.ppEnabledExtensionNames=device_ext.data();
    if(enable_validation){
        dev_desc.enabledLayerCount=validation_layers.size();
        dev_desc.ppEnabledLayerNames=validation_layers.data();
    }

    auto vkres=vkCreateDevice(adapter,&dev_desc,nullptr,&dev);
    if(vkres!=VK_SUCCESS){
        throw std::runtime_error("failed to create logical device");
    }
    return dev;
}

VkQueue get_queue(VkDevice device){
    VkQueue q;
    vkGetDeviceQueue(device,queue_family_idx,0,&q);
    return q;
}

uint32_t get_queue_index(){
    return queue_family_idx;
}

VkSwapchainKHR create_swapchain(VkDevice dev,VkSurfaceKHR surface,uint32_t w,uint32_t h,VkFormat format){
    VkSwapchainCreateInfoKHR swapchain_desc{};
    swapchain_desc.sType=VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_desc.surface=surface;
    swapchain_desc.minImageCount=3;
    swapchain_desc.imageFormat=format;
    swapchain_desc.imageExtent={w,h};
    swapchain_desc.imageArrayLayers=1;
    swapchain_desc.imageUsage=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_desc.preTransform=VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_desc.compositeAlpha=VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_desc.presentMode=VK_PRESENT_MODE_MAILBOX_KHR;
    //swapchain_desc.presentMode=VK_PRESENT_MODE_FIFO_KHR;

    VkSwapchainKHR swapchain;
    auto vkres=vkCreateSwapchainKHR(dev,&swapchain_desc,nullptr,&swapchain);
    if(vkres!=VK_SUCCESS){
        throw std::runtime_error("failed to create swap chain");
    }
    return swapchain;
}

