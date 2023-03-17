#pragma once
#include <types.h>
#include "../../buffer/buffer.h"
#include <vector>

namespace vk{

void bind_graphics_pipeline(u64 cmd,u64 pipeline_handle);
void bind_compute_pipeline(u64 cmd,u64 pipeline_handle);

using std::vector;

void bind_vertex_buffer(u64 cmd,vector<Buffer> buffers);
void bind_index_buffer(u64 cmd,Buffer index_buffer);

void push_constant(u64 cmd,u64 layout,u32 size,void* p);

}