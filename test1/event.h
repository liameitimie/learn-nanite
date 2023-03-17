#pragma once
#include <nlohmann/json_fwd.hpp>
#include <vulkan/vulkan.h>

using nlohmann::json;

struct RenderGraph;
struct Pass;

struct Event{
    enum class Type{
        pass
    }type;
    Pass* pass;

    static Event* create(RenderGraph& rdg,const json& event_desc);
    void proc_event(RenderGraph& rdg,VkCommandBuffer cmd_buffer);
};