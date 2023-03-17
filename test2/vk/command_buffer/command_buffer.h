#pragma once

#include "../types.h"
#include "../error.h"
#include <result.h>
#include "commands/drawcall.h"
#include "commands/render_pass.h"
#include "commands/bind_push.h"
#include "commands/barrier.h"

namespace vk{

struct CommandBuffer{
    u64 handle;
    u64 command_pool;

    auto new_()->Result<CommandBuffer,Error>;
    auto build()->Result<CommandBuffer,Error>;

    auto draw(
        u32 vertex_count,
        u32 instance_count,
        u32 first_vertex,
        u32 first_instance
    )->CommandBuffer{
        vk::draw(handle,vertex_count,instance_count,first_vertex,first_instance);
        return *this;
    }

    auto begin_rendering(
        RenderingInfo rendering_info
    )->CommandBuffer{
        vk::begin_rendering(handle,rendering_info);
        return *this;
    }

    auto end_rendering()->CommandBuffer{
        vk::end_rendering(handle);
        return *this;
    }

    auto bind_graphics_pipeline(u64 pipeline_handle)->CommandBuffer{
        vk::bind_graphics_pipeline(handle,pipeline_handle);
        return *this;
    }

    auto pipeline_barrier(Dependency dependency)->CommandBuffer{
        vk::pipeline_barrier(handle,dependency);
        return *this;
    }

    auto bind_vertex_buffer(vector<Buffer> buffers)->CommandBuffer{
        vk::bind_vertex_buffer(handle,buffers);
        return *this;
    }
};



}