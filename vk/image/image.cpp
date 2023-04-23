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
        .usage=usage,
        .mip_levels=1
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

Result<Image,Error> create_image2d(ImageDesc2D image_desc){
    Image image{
        .dimensions={
            .type=ImageDimensions::Dim2d,
            .width=image_desc.width,
            .height=image_desc.height
        },
        .format=image_desc.format,
        .usage=image_desc.usage,
        .mip_levels=image_desc.mip_levels,
    };
    VkImageCreateInfo img_info{
        .sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType=VK_IMAGE_TYPE_2D,
        .format=(VkFormat)image_desc.format,
        .extent={
            .width=image_desc.width,
            .height=image_desc.height,
            .depth=1
        },
        .mipLevels=image_desc.mip_levels,
        .arrayLayers=1,
        .samples=VK_SAMPLE_COUNT_1_BIT,
        .tiling=VK_IMAGE_TILING_OPTIMAL,
        .usage=image_desc.usage,
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
        .format=(VkFormat)image_desc.format,
        .subresourceRange={
            .aspectMask=aspect(image_desc.format),
            .levelCount=image_desc.mip_levels,
            .layerCount=1,
        }
    };

    res=vkCreateImageView((VkDevice)device(),&imgv_info,nullptr,(VkImageView*)&image.image_view);
    if(res!=VK_SUCCESS) return Err(Error(res));

    return Ok(image);
}

Result<ImageView,Error> Image::view(ImageViewDesc view_desc){
    VkImageViewCreateInfo imgv_info={
        .sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image=(VkImage)handle,
        .viewType=VK_IMAGE_VIEW_TYPE_2D,
        .format=(VkFormat)view_desc.format,
        .subresourceRange={
            .aspectMask=aspect(view_desc.format),
            .baseMipLevel=view_desc.mip_level,
            .levelCount=view_desc.level_count,
            .layerCount=1,
        }
    };
    ImageView image_view={
        .image=this,
        .format=view_desc.format,
        .mip_level=view_desc.mip_level,
        .level_count=view_desc.level_count
    };
    auto res=vkCreateImageView((VkDevice)device(),&imgv_info,nullptr,(VkImageView*)&image_view.handle);
    if(res!=VK_SUCCESS) return Err(Error(res));
    return Ok(image_view);
}

}