#pragma once
#include <types.h>
#include "../error.h"
#include <result.h>

namespace vk{

struct CommandBufferAllocator{
    u64 handle;

    static auto new_()->Result<CommandBufferAllocator,Error>;
};

}