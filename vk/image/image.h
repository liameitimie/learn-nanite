#pragma once
#include "dimensions.h"
#include "image_view.h"
#include "../error.h"
#include <result.h>

namespace vk{

struct ImageUsage{
    const static u32 TransferSrc = 0x00000001,
    TransferDst = 0x00000002,
    Sampled = 0x00000004,
    Storage = 0x00000008,
    ColorAttachment = 0x00000010,
    DepthStencilAttachment = 0x00000020,
    TransientAttachment = 0x00000040,
    InputAttachment = 0x00000080,
    FragmentDensityMap = 0x00000200,
    FragmentShadingRateAttachment = 0x00000100,
    AttachmentFeedbackLoop = 0x00080000;
};

struct Image{
    u64 handle;
    u64 allocation;
    u64 image_view;

    ImageDimensions dimensions;
    Format format;
    u32 usage;
    u32 mip_levels;

    Result<ImageView,Error> view(ImageViewDesc view_desc);

    static Result<Image,Error> AttachmentImage(u32 width,u32 height,Format format,u32 usage);
};

struct ImageDesc2D{
    u32 width;
    u32 height;
    Format format;
    u32 usage;
    u32 mip_levels;
};

Result<Image,Error> create_image2d(ImageDesc2D image_desc);

}