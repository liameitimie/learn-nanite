#pragma once

#include <vec_types.h>
#include <algorithm>

struct Bounds{
    vec3 pmin,pmax;
    Bounds operator+(Bounds b){
        return Bounds{
            .pmin={
                .x=std::min(pmin.x,b.pmin.x),
                .y=std::min(pmin.y,b.pmin.y),
                .z=std::min(pmin.z,b.pmin.z),
            },
            .pmax={
                .x=std::max(pmin.x,b.pmin.x),
                .y=std::max(pmin.y,b.pmin.y),
                .z=std::max(pmin.z,b.pmin.z),
            }
        };
    }
    Bounds operator+(vec3 b){
        return Bounds{
            .pmin={
                .x=std::min(pmin.x,b.x),
                .y=std::min(pmin.y,b.y),
                .z=std::min(pmin.z,b.z),
            },
            .pmax={
                .x=std::max(pmin.x,b.x),
                .y=std::max(pmin.y,b.y),
                .z=std::max(pmin.z,b.z),
            }
        };
    }
};

struct Sphere{
    vec3 o;
    f32 r;
};