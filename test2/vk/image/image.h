#pragma once
#include "dimensions.h"
#include "image_view.h"
#include "../error.h"
#include <result.h>

namespace vk{

struct Image{
    u64 handle;
    ImageDimensions dimensions;
    Format format;

    u64 image_view;
};

}