#pragma once
#include "../../types.h"
#include "../../buffer/buffer.h"
#include <vector>

namespace vk{

void bind_graphics_pipeline(u64 cmd,u64 pipeline_handle);
void bind_compute_pipeline(u64 cmd,u64 pipeline_handle);

using std::vector;

void bind_vertex_buffer(u64 cmd,vector<Buffer> buffers);

}