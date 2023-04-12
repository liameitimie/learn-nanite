#pragma once
#include <types.h>
#include "../shader.h"

namespace vk{

struct ComputePipeline{
    u64 handle;
    u64 pipeline_layout;
    ShaderModule compute_shader_;
    u32 push_constant_size_;

    static auto new_()->ComputePipeline{
        return ComputePipeline{
            .compute_shader_={},
            .push_constant_size_=0,
        };
    }

    auto build()->Result<ComputePipeline,Error>;

    auto compute_shader(
        ShaderModule compute_shader_
    )->ComputePipeline&{
        this->compute_shader_=compute_shader_;
        return *this;
    }

    auto push_constant_size(u32 push_constant_size_)->ComputePipeline{
        this->push_constant_size_=push_constant_size_;
        return *this;
    }
};

}