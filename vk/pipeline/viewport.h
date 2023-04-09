#pragma once
#include <vec_types.h>
#include <utility>
#include <vector>


namespace vk{

struct Viewport{
    vec2 origin;
    vec2 dimensions;
    vec2 depth_range;

    static Viewport dimension(f32 x,f32 y){
        return Viewport{
            .origin={.x=0.0f,.y=0.0f},
            .dimensions={.x=x,.y=y},
            .depth_range={.x=0.0f,.y=1.0f}
        };
    }
};

struct Scissor{
    ivec2 origin;
    uvec2 dimensions;
    
    static auto irrelevant()->Scissor{
        return Scissor {
            .origin={0,0},
            .dimensions={0x7fffffff,0x7fffffff},
        };
    }
};


using std::vector;
using std::pair;

struct ViewportState{
    vector<pair<Viewport,Scissor>> data;

    static auto new_()->ViewportState{
        return ViewportState{};
    }
    static auto Default(vector<Viewport> data)->ViewportState{
        return viewport_fixed_scissor_irrelevant(data);
    }
    static auto viewport_fixed_scissor_irrelevant(vector<Viewport> data)->ViewportState{
        ViewportState state;
        state.data.resize(data.size());
        u32 i=0;
        for(auto&[viewport,scissor]:state.data){
            viewport=data[i];
            scissor=Scissor::irrelevant();
            i++;
        }
        return state;
    }
};

}