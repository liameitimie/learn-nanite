#pragma once
#include <types.h>
#include "error.h"
#include <result.h>

namespace vk{

struct ShaderStage{
    const static u32 Vertex=0x00000001;
    const static u32 TessellationControl=0x00000002;
    const static u32 TessellationEvaluation=0x00000004;
    const static u32 Geometry=0x00000008;
    const static u32 Fragment=0x00000010;
    const static u32 Compute=0x00000020;
    const static u32 Graphics=0x0000001F;
    const static u32 All=0x7FFFFFFF;
};

struct ShaderModule{
    u64 handle; // VkShaderModule

    static Result<ShaderModule,Error> from_file(const char*);
};

}