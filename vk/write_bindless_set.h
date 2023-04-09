#pragma once
#include "buffer/buffer.h"
#include "descriptor_set.h"

namespace vk{

void write_bindless_set(u32 idx,Buffer buffer,DescriptorType type);

void write_bindless_set(u32 idx,Buffer* buffer,u32 buffer_count,DescriptorType type);


}