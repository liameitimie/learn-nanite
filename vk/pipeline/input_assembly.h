#pragma once

namespace vk{

enum class PrimitiveTopology {
    PointList = 0,
    LineList = 1,
    LineStrip = 2,
    TriangleList = 3,
    TriangleStrip = 4,
    TriangleFan = 5,
    LineListWithAdjacency = 6,
    LineStripWithAdjacency = 7,
    TriangleListWithAdjacency = 8,
    TriangleStripWithAdjacency = 9,
    PatchList = 10,
};

struct InputAssemblyState{
    PrimitiveTopology topology;

    static auto new_()->InputAssemblyState{
        return InputAssemblyState{PrimitiveTopology::TriangleList};
    }
};


}// namespace vk