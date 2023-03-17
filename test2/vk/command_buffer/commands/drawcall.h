#pragma once
#include "../../types.h"

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



}