#pragma once
#include "../../image/image.h"
#include "../../image/layout.h"
#include <vector>

namespace vk{

enum class AccessFlag {
    INDIRECT_COMMAND_READ = 0x00000001,
    INDEX_READ = 0x00000002,
    VERTEX_ATTRIBUTE_READ = 0x00000004,
    UNIFORM_READ = 0x00000008,
    INPUT_ATTACHMENT_READ = 0x00000010,
    SHADER_READ = 0x00000020,
    SHADER_WRITE = 0x00000040,
    COLOR_ATTACHMENT_READ = 0x00000080,
    COLOR_ATTACHMENT_WRITE = 0x00000100,
    DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200,
    DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400,
    TRANSFER_READ = 0x00000800,
    TRANSFER_WRITE = 0x00001000,
    HOST_READ = 0x00002000,
    HOST_WRITE = 0x00004000,
    MEMORY_READ = 0x00008000,
    MEMORY_WRITE = 0x00010000,
    NONE = 0,
    TRANSFORM_FEEDBACK_WRITE_EXT = 0x02000000,
    TRANSFORM_FEEDBACK_COUNTER_READ_EXT = 0x04000000,
    TRANSFORM_FEEDBACK_COUNTER_WRITE_EXT = 0x08000000,
    CONDITIONAL_RENDERING_READ_EXT = 0x00100000,
    COLOR_ATTACHMENT_READ_NONCOHERENT_EXT = 0x00080000,
    ACCELERATION_STRUCTURE_READ_KHR = 0x00200000,
    ACCELERATION_STRUCTURE_WRITE_KHR = 0x00400000,
    FRAGMENT_DENSITY_MAP_READ_EXT = 0x01000000,
    FRAGMENT_SHADING_RATE_ATTACHMENT_READ_KHR = 0x00800000,
    COMMAND_PREPROCESS_READ_NV = 0x00020000,
    COMMAND_PREPROCESS_WRITE_NV = 0x00040000,
};

struct PipelineStage {
    static const u32 TOP_OF_PIPE = 0x00000001,
    DRAW_INDIRECT = 0x00000002,
    VERTEX_INPUT = 0x00000004,
    VERTEX_SHADER = 0x00000008,
    TESSELLATION_CONTROL_SHADER = 0x00000010,
    TESSELLATION_EVALUATION_SHADER = 0x00000020,
    GEOMETRY_SHADER = 0x00000040,
    FRAGMENT_SHADER = 0x00000080,
    EARLY_FRAGMENT_TESTS = 0x00000100,
    LATE_FRAGMENT_TESTS = 0x00000200,
    COLOR_ATTACHMENT_OUTPUT = 0x00000400,
    COMPUTE_SHADER = 0x00000800,
    TRANSFER = 0x00001000,
    BOTTOM_OF_PIPE = 0x00002000,
    HOST = 0x00004000,
    ALL_GRAPHICS = 0x00008000,
    ALL_COMMANDS = 0x00010000,
    NONE = 0,
    TRANSFORM_FEEDBACK_EXT = 0x01000000,
    CONDITIONAL_RENDERING_EXT = 0x00040000,
    ACCELERATION_STRUCTURE_BUILD_KHR = 0x02000000,
    RAY_TRACING_SHADER_KHR = 0x00200000,
    FRAGMENT_DENSITY_PROCESS_EXT = 0x00800000,
    FRAGMENT_SHADING_RATE_ATTACHMENT_KHR = 0x00400000,
    COMMAND_PREPROCESS_NV = 0x00020000,
    TASK_SHADER_EXT = 0x00080000,
    MESH_SHADER_EXT = 0x00100000;
};

struct ImageBarrier{
    Image image;
    u32 src_stage;
    AccessFlag src_access;
    u32 dst_stage;
    AccessFlag dst_access;
    ImageLayout old_layout;
    ImageLayout new_layout;
};

using std::vector;

struct Dependency{
    vector<ImageBarrier> image_barriers;
};

void pipeline_barrier(u64 cmd,Dependency dependency);

}