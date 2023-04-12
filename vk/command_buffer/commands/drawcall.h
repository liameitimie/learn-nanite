#pragma once
#include <types.h>
#include "../../buffer/buffer.h"

namespace vk{

struct CommandBuffer;

void dispatch(
    u64 cmd,
    u32 x,u32 y,u32 z
);
void draw(
    u64 cmd,
    u32 vertex_count,
    u32 instance_count,
    u32 first_vertex,
    u32 first_instance
);

void draw_indexed(
    u64 cmd,
    u32 index_count,
    u32 instance_count,
    u32 first_vertex,
    i32 vertex_offset,
    u32 first_instance
);

void dispatch(u64 cmd,u32 x,u32 y,u32 z);

void draw_indirect(u64 cmd,Buffer buffer);

}