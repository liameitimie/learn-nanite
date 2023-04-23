#pragma once
#include <types.h>
#include <result.h>
#include "error.h"

namespace vk{

struct SamplerDesc{
    bool unnorm_coord;
};

struct Sampler{
    u64 handle;
    bool unnorm_coord;
};

Result<Sampler,Error> create_sampler(SamplerDesc sampler_desc);

}