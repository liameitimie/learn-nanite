#pragma once

#include <types.h>
#include "../error.h"
#include <result.h>
#include "commands/drawcall.h"
#include "commands/render_pass.h"
#include "commands/bind_push.h"
#include "commands/barrier.h"
#include "command_buffer_allocator.h"
#include "commands/set_state.h"

namespace vk{

enum class CommandBufferUsage{
    OneTimeSubmit = 0x00000001,
    //VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT = 0x00000002,
    SimultaneousUse = 0x00000004,
};

enum class IndexType{
    UINT16 = 0,
    UINT32 = 1,
    NONE = 1000165000,
    UINT8 = 1000265000,
};

struct CommandBuffer{
    u64 handle;
    CommandBufferAllocator allocator;

    static auto new_(CommandBufferAllocator allocator)->Result<CommandBuffer,Error>;

    auto begin(CommandBufferUsage usage)->Result<CommandBuffer,Error>;
    auto build()->Result<CommandBuffer,Error>;

    auto reset()->Result<CommandBuffer,Error>;

    auto draw(
        u32 vertex_count,
        u32 instance_count,
        u32 first_vertex,
        u32 first_instance
    )->CommandBuffer{
        vk::draw(handle,vertex_count,instance_count,first_vertex,first_instance);
        return *this;
    }

    auto draw_indexed(
        u32 index_count,
        u32 instance_count,
        u32 first_vertex,
        i32 vertex_offset,
        u32 first_instance
    )->CommandBuffer{
        vk::draw_indexed(handle,index_count,instance_count,first_vertex,vertex_offset,first_instance);
        return *this;
    }

    auto draw_indirect(Buffer buffer)->CommandBuffer{
        vk::draw_indirect(handle,buffer);
        return *this;
    }

    auto dispatch(u32 x,u32 y,u32 z)->CommandBuffer{
        vk::dispatch(handle,x,y,z);
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

    auto bind_compute_pipeline(u64 pipeline_handle)->CommandBuffer{
        vk::bind_compute_pipeline(handle,pipeline_handle);
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

    auto bind_index_buffer(Buffer index_buffer)->CommandBuffer{
        vk::bind_index_buffer(handle,index_buffer);
        return *this;
    }

    auto push_constant(u64 layout,u32 size,void* data)->CommandBuffer{
        vk::push_constant(handle,layout,size,data);
        return *this;
    }

    auto bind_descriptor_sets(PipelineBindPoint binding_point,u64 layout,u32 idx,u64 desc_set)->CommandBuffer{
        vk::bind_descriptor_sets(handle,binding_point,layout,idx,desc_set);
        return *this;
    }

    auto set_viewport(std::vector<Viewport> viewports)->CommandBuffer{
        vk::set_viewport(handle,viewports);
        return *this;
    }

    auto blit_image(Image img1,Image img2,ivec4 img1_range,ivec4 img2_range)->CommandBuffer;
};



}