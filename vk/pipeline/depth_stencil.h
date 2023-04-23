#pragma once

#include <option.h>

namespace vk{

enum class CompareOp{
    Never = 0,
    Less = 1,
    Equal = 2,
    LessOrEqual = 3,
    Greater = 4,
    NotEqual = 5,
    GreaterOrEqual = 6,
    Always = 7,
};

struct DepthState{
    bool write_enable;
    CompareOp compare_op;
};

struct DepthStencilState{
    Option<DepthState> depth;

    static auto disable()->DepthStencilState{
        return DepthStencilState{
            .depth=None()
        };
    }

    static auto always()->DepthStencilState{
        return DepthStencilState{
            .depth=Some(DepthState{
                .write_enable=true,
                .compare_op=CompareOp::Always
            })
        };
    }

    static auto simple_depth_test()->DepthStencilState{
        return DepthStencilState{
            .depth=Some(DepthState{
                .write_enable=true,
                .compare_op=CompareOp::Less,
            })
        };
    }

    static auto reverse_z_test()->DepthStencilState{
        return DepthStencilState{
            .depth=Some(DepthState{
                .write_enable=true,
                .compare_op=CompareOp::Greater,
            })
        };
    }
};

}// namespace vk