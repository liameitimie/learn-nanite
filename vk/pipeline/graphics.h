#pragma once
#include "render_pass.h"
#include "vertex_input.h"
#include "input_assembly.h"
#include "viewport.h"
#include "rasterization.h"
#include "depth_stencil.h"
#include "dynamic_state.h"
#include "../shader.h"
#include "../error.h"
#include <result.h>

namespace vk{

struct GraphicsPipeline{
    u64 handle;
    u64 pipeline_layout;
    
    RenderingCreateInfo render_pass_;

    ShaderModule vertex_shader_;
    ShaderModule fragment_shader_;

    u32 push_constant_size_;
    vector<VertexBufferDescription> vertex_input_state_;
    InputAssemblyState input_assembly_state_;
    ViewportState viewport_state_;
    RasterizationState rasterization_state_;
    DepthStencilState depth_stencil_state_;

    vector<DynamicState> dynamic_state_;

    static auto new_()->GraphicsPipeline{
        return GraphicsPipeline{
            .render_pass_={},
            .vertex_shader_={},
            .fragment_shader_={},
            .push_constant_size_=0,
            .input_assembly_state_=InputAssemblyState::new_(),
            .viewport_state_=ViewportState::new_(),
            .rasterization_state_=RasterizationState::new_(),
            .depth_stencil_state_=DepthStencilState::disable()
        };
    }

    auto build()->Result<GraphicsPipeline,Error>;

    auto render_pass(
        RenderingCreateInfo render_pass_
    )->GraphicsPipeline&{
        this->render_pass_=render_pass_;
        return *this;
    }

    auto vertex_shader(
        ShaderModule vertex_shader_
    )->GraphicsPipeline&{
        this->vertex_shader_=vertex_shader_;
        return *this;
    }

    auto fragment_shader(
        ShaderModule fragment_shader_
    )->GraphicsPipeline&{
        this->fragment_shader_=fragment_shader_;
        return *this;
    }

    auto push_constant_size(u32 push_constant_size_)->GraphicsPipeline{
        this->push_constant_size_=push_constant_size_;
        return *this;
    }

    auto vertex_input_state(
        vector<VertexBufferDescription> vertex_input_state_
    )->GraphicsPipeline&{
        this->vertex_input_state_=vertex_input_state_;
        return *this;
    }

    auto input_assembly_state(
        InputAssemblyState input_assembly_state_
    )->GraphicsPipeline&{
        this->input_assembly_state_=input_assembly_state_;
        return *this;
    }

    auto input_assembly_state(
        PrimitiveTopology primitive_topology
    )->GraphicsPipeline&{
        this->input_assembly_state_.topology=primitive_topology;
        return *this;
    }

    auto viewport_state(
        ViewportState viewport_state_
    )->GraphicsPipeline&{
        this->viewport_state_=viewport_state_;
        return *this;
    }

    auto rasterization_state(
        RasterizationState rasterization_state_
    )->GraphicsPipeline&{
        this->rasterization_state_=rasterization_state_;
        return *this;
    }

    auto depth_stencil_state(
        DepthStencilState depth_stencil_state_
    )->GraphicsPipeline&{
        this->depth_stencil_state_=depth_stencil_state_;
        return *this;
    }

    auto dynamic_state(
        vector<DynamicState> dynamic_state_
    )->GraphicsPipeline&{
        this->dynamic_state_=dynamic_state_;
        return *this;
    }
};

}// namespace vk