#pragma once
#include <types.h>
#include "error.h"
#include <result.h>
#include <vector>
#include "command_buffer/command_buffer.h"

namespace vk{

struct Fence{
    u64 handle;
};

Result<Fence,Error> create_fence(bool signaled);
void wait_for_fence(Fence fence);
void reset_fence(Fence fence);

struct Semaphore{
    u64 handle;
};

Result<Semaphore,Error> create_semaphore();

using std::vector;

struct SubmitInfo{
    vector<Semaphore> waiting;
    vector<CommandBuffer> command_buffers;
    vector<Semaphore> signal;
};

void queue_submit(SubmitInfo submit_info,Fence fence);

struct PresentInfo{
    vector<Semaphore> waiting;
    u32 swapchain_image_idx;
};

void queue_present(PresentInfo present_info);

}