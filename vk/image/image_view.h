#pragma once
#include "dimensions.h"

namespace vk{

struct ImageViewDesc{
    Format format;
    u32 mip_level;
    u32 level_count;
};

struct Image;

struct ImageView{
    u64 handle;
    Image* image;
    Format format;
    u32 mip_level;
    u32 level_count;
};

}