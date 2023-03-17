#pragma once

#include <vulkan/vulkan.h>
#include <nlohmann/json_fwd.hpp>
//#include <vector>

using nlohmann::json;
//using std::vector;

struct RenderGraph;
struct Resource;

struct Pass{
    enum Type{
        Graphic,
        Compute
    }type;
    VkPipeline pipeline;
    Resource* attachment;

    static Pass* create(RenderGraph& rdg,const json& pass_desc);
    void proc_pass(RenderGraph& rdg,VkCommandBuffer cmd_buffer);
};