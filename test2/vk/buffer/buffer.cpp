#include "buffer.h"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include "../vk_context.h"

namespace vk{

VmaAllocation t;

auto Buffer::from_raw(
    BufferAllocateInfo info,
    void* p,
    u64 size
)->Result<Buffer,Error>{

    VkBufferCreateInfo buffer_desc={
        .sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size=size,
        .usage=info.buffer_usage
    };
    VmaAllocationCreateInfo alloc_info={
        .usage=(info.memory_usage==MemoryUsage::GpuOnly
                ?VMA_MEMORY_USAGE_GPU_ONLY
                :(info.memory_usage==MemoryUsage::Upload
                    ?VMA_MEMORY_USAGE_CPU_TO_GPU
                    :VMA_MEMORY_USAGE_GPU_TO_CPU))
    };
    Buffer buffer={
        .size=size,
        .buffer_usage=info.buffer_usage,
        .memory_usage=info.memory_usage
    };
    auto res=vmaCreateBuffer((VmaAllocator)allocator(),&buffer_desc,&alloc_info,
        (VkBuffer*)&buffer.handle,
        (VmaAllocation*)&buffer.allocation,
        nullptr);
    if(res!=VK_SUCCESS) return Err(Error(res));

    void* data;
    vmaMapMemory((VmaAllocator)allocator(),(VmaAllocation)buffer.allocation,&data);
    memcpy(data,p,size);
    vmaUnmapMemory((VmaAllocator)allocator(),(VmaAllocation)buffer.allocation);
    return Ok(buffer);
}

}