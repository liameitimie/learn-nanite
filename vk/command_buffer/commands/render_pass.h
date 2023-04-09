#pragma once
#include <option.h>
#include <vec_types.h>
#include "../../image/layout.h"
#include <vector>

namespace vk{

using std::vector;

enum class LoadOp{
    Load = 0,
    Clear = 1,
    DontCare = 2,
};

enum class StoreOp{
    Store = 0,
    DontCare = 1,
};

union ClearColorValue{
    vec4 f_data;
    ivec4 i_data;
    uvec4 u_data;

    ClearColorValue(vec4 data){f_data=data;}
    ClearColorValue(ivec4 data){i_data=data;}
    ClearColorValue(uvec4 data){u_data=data;}
};
struct ClearDepthStencilValue {
    f32 depth;
    u32 stencil;
};

struct ClearValue{
    union{
        ClearColorValue color;
        ClearDepthStencilValue depth_stencil;
    };
    ClearValue(){}
    ClearValue(ClearColorValue color){
        this->color=color;
    }
    ClearValue(ClearDepthStencilValue depth_stencil){
        this->depth_stencil=depth_stencil;
    }
};

struct RenderingAttachmentInfo{
    u64 image_view;
    LoadOp load_op;
    StoreOp store_op;
    //Option<ClearValue> clear_value;
};

struct RenderingInfo{
    ivec2 render_area_offset;
    uvec2 render_area_extent;
    vector<RenderingAttachmentInfo> color_attachments;
    Option<RenderingAttachmentInfo> depth_attachment;
    Option<RenderingAttachmentInfo> stencil_attachment;
};

void begin_rendering(u64 cmd,RenderingInfo rendering_info);
void end_rendering(u64 cmd);

}