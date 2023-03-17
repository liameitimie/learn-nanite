#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

#define VK_CHECK(x){\
    VkResult vk_res=x;\
    if(vk_res!=VK_SUCCESS){\
        std::cout<<"vulkan error in "#x<<", VkResult: "<<vk_res<<std::endl;\
        exit(0);\
    }\
}