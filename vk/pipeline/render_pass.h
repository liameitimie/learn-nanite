#pragma once
#include <types.h>
#include "../format.h"
#include <vector>

namespace vk{

using std::vector;

struct RenderingCreateInfo{
    vector<Format> color_attachment_formats;
    Format depth_attachment_format;
    Format stencil_attachment_format;
};

}