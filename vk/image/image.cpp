#include "image.h"
#include <vulkan/vulkan.h>
#include "../memory_allocator.h"
#include "../vk_context.h"
#include "aspect.h"
#include "layout.h"

namespace vk{

Result<Image,Error> Image::AttachmentImage(u32 width,u32 height,Format format,u32 usage){
    Image image{
        .dimensions={
            .type=ImageDimensions::Dim2d,
            .width=width,
            .height=height
        },
        .format=format,
        .usage=usage
    };

    VkImageCreateInfo img_info{
        .sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType=VK_IMAGE_TYPE_2D,
        .format=(VkFormat)format,
        .extent={
            .width=width,
            .height=height,
            .depth=1
        },
        .mipLevels=1,
        .arrayLayers=1,
        .samples=VK_SAMPLE_COUNT_1_BIT,
        .tiling=VK_IMAGE_TILING_OPTIMAL,
        .usage=usage,
    };
    VmaAllocationCreateInfo alloc_info{
        .usage=VMA_MEMORY_USAGE_GPU_ONLY,
    };
    auto res=vmaCreateImage((VmaAllocator)allocator(),&img_info,&alloc_info,
        (VkImage*)&image.handle,
        (VmaAllocation*)&image.allocation,
        nullptr);

    if(res!=VK_SUCCESS) return Err(Error(res));

    VkImageViewCreateInfo imgv_info={
        .sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image=(VkImage)image.handle,
        .viewType=VK_IMAGE_VIEW_TYPE_2D,
        .format=(VkFormat)format,
        .subresourceRange={
            .aspectMask=aspect(format),
            .levelCount=1,
            .layerCount=1,
        }
    };

    res=vkCreateImageView((VkDevice)device(),&imgv_info,nullptr,(VkImageView*)&image.image_view);
    if(res!=VK_SUCCESS) return Err(Error(res));

    return Ok(image);
}

}