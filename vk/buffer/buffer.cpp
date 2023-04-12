#include "buffer.h"
#include "../memory_allocator.h"
#include "../vk_context.h"

namespace vk{

VmaAllocation t;

auto Buffer::from_raw(
    BufferAllocateInfo info,
    void* p,
    u64 size,
    u32 stride
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
        .stride=stride,
        .buffer_usage=info.buffer_usage,
        .memory_usage=info.memory_usage
    };
    auto res=vmaCreateBuffer((VmaAllocator)allocator(),&buffer_desc,&alloc_info,
        (VkBuffer*)&buffer.handle,
        (VmaAllocation*)&buffer.allocation,
        nullptr);
    if(res!=VK_SUCCESS) return Err(Error(res));

    if(p){
        void* data;
        auto res=vmaMapMemory((VmaAllocator)allocator(),(VmaAllocation)buffer.allocation,&data);
        if(res!=VK_SUCCESS) return Err(Error(res));
        memcpy(data,p,size);
        vmaUnmapMemory((VmaAllocator)allocator(),(VmaAllocation)buffer.allocation);
    }
    
    return Ok(buffer);
}

void Buffer::update(void* p,u64 size){
    void* data;
    vmaMapMemory((VmaAllocator)allocator(),(VmaAllocation)allocation,&data);
    memcpy(data,p,size);
    vmaUnmapMemory((VmaAllocator)allocator(),(VmaAllocation)allocation);
}

void read_from_buffer(void* p,Buffer buffer,u64 size){
    void* data;
    vmaMapMemory((VmaAllocator)allocator(),(VmaAllocation)buffer.allocation,&data);
    memcpy(p,data,size);
    vmaUnmapMemory((VmaAllocator)allocator(),(VmaAllocation)buffer.allocation);
}

}