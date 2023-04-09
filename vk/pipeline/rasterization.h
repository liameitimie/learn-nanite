#pragma once

namespace vk{

enum class PolygonMode{
    Fill = 0,
    Line = 1,
    Point = 2
};

enum class CullMode{
    None = 0,
    Front = 0x00000001,
    Back = 0x00000002,
    FrontAndBack = 0x00000003,
};

enum class FrontFace{
    CounterClockwise = 0,
    Clockwise = 1,
};

struct RasterizationState{
    bool depth_clamp_enable;
    bool rasterizer_discard_enable;
    PolygonMode polygon_mode;
    CullMode cull_mode;
    FrontFace front_face;

    static auto new_()->RasterizationState{
        return RasterizationState{
            .depth_clamp_enable=false,
            .rasterizer_discard_enable=false,
            .polygon_mode=PolygonMode::Fill,
            .cull_mode=CullMode::None,
            .front_face=FrontFace::CounterClockwise
        };
    }

    auto polygon_mode_(PolygonMode polygon_mode)->RasterizationState{
        this->polygon_mode=polygon_mode;
        return *this;
    }
    auto cull_mode_(CullMode cull_mode)->RasterizationState{
        this->cull_mode=cull_mode;
        return *this;
    }
    auto front_face_(FrontFace front_face)->RasterizationState{
        this->front_face=front_face;
        return *this;
    }
};

}// namespace vk