#include "vk/vk.h"
#include "engine.h"

vk::GraphicsPipeline create_pipeline(Engine &engine);

vk::CommandBuffer create_cmd(Engine &engine,u32 swapchain_idx,vk::GraphicsPipeline pipeline);