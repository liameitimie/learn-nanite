#pragma once
#include "dimensions.h"

namespace vk{

struct ImageView{
    u64 handle;
    u64 image;
    ImageDimensions dimensions;
    Format format;
};

}