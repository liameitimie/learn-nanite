#pragma once
#include <types.h>
#include "image/image.h"

struct GLFWwindow;

namespace vk{

void init();
void init_surface(u64 handle,u32 width,u32 height);

u64 instance();
u64 physical_device();
u64 device();
u64 queue();
u32 queue_family();
u64 swapchain();

u64 allocator();

u32 num_swapchain_image();
Format swapchain_image_format();
Image swapchain_image(u32 idx);

u64 bindless_buffer_layout();
u64 bindless_buffer_set();

u64 bindless_image_layout();
u64 bindless_image_set();

// u64 default_sampler();

void cleanup();

}