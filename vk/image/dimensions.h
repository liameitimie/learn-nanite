#pragma once
#include <vec_types.h>
#include "../format.h"

namespace vk{

struct ImageDimensions{
    enum{
        Dim1d,
        Dim2d,
        Dim3d,
    }type;
    u32 width;
    u32 height;
    u32 depth;
    u32 array_layers;
};

}