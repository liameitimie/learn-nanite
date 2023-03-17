#pragma once
#include <stdint.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <nlohmann/json_fwd.hpp>
#include "vk_util.h"
#include <string>
#include <vector>

using std::vector;
using std::string;
using nlohmann::json;

struct Resource;
struct Pass;
struct Event;
struct RenderGraphImpl;

struct RenderGraph{
    void init();
    void from_json_file(string path);
    void from_json(const json& desc);
    void add_resource();
    void add_pass(const json& pass_desc);
    void add_event(const json& event_desc);
    void build();

    void render();
    void clear();
    
    RenderGraphImpl* impl;

    VkDevice device();
    GLFWwindow* window();
    uint32_t width();
    uint32_t height();

    Resource* get_resource(string name);
    Pass* get_pass(string name);
    const vector<Event*>& events();
};