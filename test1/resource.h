#pragma once

#include <vulkan/vulkan.h>

struct Resource{
    enum Type{
        Image
    }type;
    union{
        struct{
            VkFormat format;
            VkImage image;
            VkImageView image_view;
        };
    };
};