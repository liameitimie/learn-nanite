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

void draw_indexed(
    u64 cmd,
    u32 index_count,
    u32 instance_count,
    u32 first_vertex,
    i32 vertex_offset,
    u32 first_instance
){
    vkCmdDrawIndexed((VkCommandBuffer)cmd,index_count,instance_count,first_vertex,vertex_offset,first_instance);
}

void dispatch(u64 cmd,u32 x,u32 y,u32 z){
    vkCmdDispatch((VkCommandBuffer)cmd,x,y,z);
}

void draw_indirect(u64 cmd,Buffer buffer){
    vkCmdDrawIndirect((VkCommandBuffer)cmd,(VkBuffer)buffer.handle,0,1,4*sizeof(u32));
}

}