#include "drawcall.h"
#include <vulkan/vulkan.h>

namespace vk{

void draw(
    u64 cmd,
    u32 vertex_count,
    u32 instance_count,
    u32 first_vertex,
    u32 first_instance
){
    vkCmdDraw((VkCommandBuffer)cmd,vertex_count,instance_count,first_vertex,first_instance);
}

}