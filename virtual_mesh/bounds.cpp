#pragma once

#include "bounds.h"
#include <algorithm>
#include <vec_math.h>
#include <assert.h>

Bounds Bounds::operator+(Bounds b){
    Bounds bounds;
    bounds.pmin={
        .x=std::min(pmin.x,b.pmin.x),
        .y=std::min(pmin.y,b.pmin.y),
        .z=std::min(pmin.z,b.pmin.z),
    };
    bounds.pmax={
        .x=std::max(pmax.x,b.pmax.x),
        .y=std::max(pmax.y,b.pmax.y),
        .z=std::max(pmax.z,b.pmax.z),
    };
    return bounds;
}

Bounds Bounds::operator+(vec3 b){
    Bounds bounds;
    bounds.pmin={
        .x=std::min(pmin.x,b.x),
        .y=std::min(pmin.y,b.y),
        .z=std::min(pmin.z,b.z),
    };
    bounds.pmax={
        .x=std::max(pmax.x,b.x),
        .y=std::max(pmax.y,b.y),
        .z=std::max(pmax.z,b.z),
    };
    return bounds;
}

Sphere Sphere::from_points(vec3* pos,u32 size){
    u32 min_idx[3]={};
    u32 max_idx[3]={};
    for(u32 i=0;i<size;i++){
        for(int k=0;k<3;k++){
            if(pos[i][k]<pos[min_idx[k]][k]) min_idx[k]=i;
            if(pos[i][k]>pos[max_idx[k]][k]) max_idx[k]=i;
        }
    }
    f32 max_len=0;
    u32 max_axis=0;
    for(u32 k=0;k<3;k++){
        vec3 pmin=pos[min_idx[k]];
        vec3 pmax=pos[max_idx[k]];
        f32 tlen=length2(pmax-pmin);
        if(tlen>max_len) max_len=tlen,max_axis=k;
    }
    vec3 pmin=pos[min_idx[max_axis]];
    vec3 pmax=pos[max_idx[max_axis]];

    Sphere sphere;
    sphere.center=(pmin+pmax)*0.5f;
    sphere.radius=f32(0.5*sqrt(max_len));
    max_len=sphere.radius*sphere.radius;

    for(u32 i=0;i<size;i++){
        f32 len=length2(pos[i]-sphere.center);
        if(len>max_len){
            len=sqrt(len);
            f32 t=0.5-0.5*(sphere.radius/len);
            sphere.center=sphere.center+(pos[i]-sphere.center)*t;
            sphere.radius=(sphere.radius+len)*0.5;
            max_len=sphere.radius*sphere.radius;
        }
    }
    //
    for(u32 i=0;i<size;i++){
        f32 len=length(pos[i]-sphere.center);
        assert(len-1e-6<=sphere.radius);
    }
    return sphere;
}

inline f32 sqr(f32 x){return x*x;}

Sphere Sphere::operator+(Sphere b){
    vec3 t=b.center-center;
    f32 tlen2=length2(t);
    if(sqr(radius-b.radius)>=tlen2){
        return radius<b.radius?b:*this;
    }
    Sphere sphere;
    f32 tlen=sqrt(tlen2);
    sphere.radius=(tlen+radius+b.radius)*0.5;
    sphere.center=center+t*((sphere.radius-radius)/tlen);
    return sphere;
}

Sphere Sphere::from_spheres(Sphere* spheres,u32 size){
    u32 min_idx[3]={};
    u32 max_idx[3]={};
    for(u32 i=0;i<size;i++){
        for(u32 k=0;k<3;k++){
            if(spheres[i].center[k]-spheres[i].radius < spheres[min_idx[k]].center[k]-spheres[min_idx[k]].radius)
                min_idx[k]=i;
            if(spheres[i].center[k]+spheres[i].radius < spheres[max_idx[k]].center[k]+spheres[max_idx[k]].radius)
                max_idx[k]=i;
        }
    }
    f32 max_len=0;
    u32 max_axis=0;
    for(u32 k=0;k<3;k++){
        Sphere spmin=spheres[min_idx[k]];
        Sphere spmax=spheres[max_idx[k]];
        f32 tlen=length(spmax.center-spmin.center)+spmax.radius+spmin.radius;
        if(tlen>max_len) max_len=tlen,max_axis=k;
    }
    Sphere sphere=spheres[min_idx[max_axis]];
    sphere=sphere+spheres[max_idx[max_axis]];
    for(u32 i=0;i<size;i++){
        sphere=sphere+spheres[i];
    }
    //
    for(u32 i=0;i<size;i++){
        f32 t1=sqr(sphere.radius-spheres[i].radius);
        f32 t2=length2(sphere.center-spheres[i].center);
        assert(t1+1e-6>=t2);
    }
    return sphere;
}