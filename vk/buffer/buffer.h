#pragma once
#include <types.h>
#include "../error.h"
#include <result.h>
#include <vector>

namespace vk{

struct BufferUsage{
    const static u32 TransferSrc = 0x00000001,
    TransferDst = 0x00000002,
    UniformTexelBuffer = 0x00000004,
    StorageTexelBuffer = 0x00000008,
    UniformBuffer = 0x00000010,
    StorageBuffer = 0x00000020,
    IndexBuffer = 0x00000040,
    VertexBuffer = 0x00000080,
    IndirectBuffer = 0x00000100,
    ShaderDeviceAddress = 0x00020000;
};

enum class MemoryUsage{
    GpuOnly,
    Upload,
    Download
};

struct BufferAllocateInfo{
    u32 flags;
    u32 buffer_usage;
    MemoryUsage memory_usage;
};

using std::vector;

struct Buffer{
    u64 handle;
    u64 allocation;
    u64 size;
    u32 stride;
    u32 buffer_usage;
    MemoryUsage memory_usage;
    
    static auto from_raw(
        BufferAllocateInfo info,
        void* p,
        u64 size,
        u32 stride
    )->Result<Buffer,Error>;

    template<class T>
    static auto from_iter(
        BufferAllocateInfo info,
        vector<T> data
    )->Result<Buffer,Error>{
        u64 size=data.size()*sizeof(T);
        u32 stride=sizeof(T);
        return from_raw(info,data.data(),size,stride);
    }

    void update(void* p,u64 size);
};


}