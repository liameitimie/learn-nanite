#pragma once
#include <types.h>
#include "../format.h"
#include <vector>

namespace vk{

enum class VertexInputRate{
    Vertex = 0,
    Instance = 1,
};

struct VertexMemberInfo{
    u32 offset;
    Format format;
};

using std::vector;

struct VertexBufferDescription{
    vector<VertexMemberInfo> member;
    u32 stride;
    VertexInputRate input_rate;
};

// class Vertex{
// public:
//     virtual auto per_vertex()->VertexBufferDescription=0;
//     virtual auto per_instance()->VertexBufferDescription=0;
// };


}