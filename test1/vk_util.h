#pragma once

#include <vulkan/vulkan.h>

#define VK_CHECK(x){\
    VkResult vk_res=x;\
    if(vk_res!=VK_SUCCESS){\
        cout<<"vulkan error in "#x<<", VkResult: "<<vk_res<<endl;\
        exit(0);\
    }\
}