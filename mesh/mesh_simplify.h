#pragma once
#include <vec_types.h>

class MeshSimplifier{
    MeshSimplifier* impl;
public:
    MeshSimplifier(){impl=nullptr;}
    MeshSimplifier(vec3* verts,u32 num_vert,u32* indexes,u32 num_index);
    ~MeshSimplifier();

    void lock_position(vec3 p);
    // bool is_position_locked(vec3 p);
    void simplify(u32 target_num_tri);
    u32 remaining_num_vert();
    u32 remaining_num_tri();
    f32 max_error();
};