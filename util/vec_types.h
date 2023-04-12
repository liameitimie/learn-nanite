#pragma once
#include <types.h>

struct vec2{
    f32 x,y;
};

struct dvec2{
    f64 x,y;
};

struct uvec2{
    u32 x,y;
    bool operator==(const uvec2& b)const{
        return x==b.x&&y==b.y;
    }
    bool operator<(const uvec2& b)const{
        return x!=b.x?x<b.x:y<b.y;
    }
};

struct ivec2{
    i32 x,y;
};

struct vec3{
    f32 x,y,z;
    bool operator==(const vec3& b)const{
        return x==b.x&&y==b.y&&z==b.z;
    }
    f32& operator[](u32 i){
        return ((f32*)this)[i];
    }
};

struct dvec3{
    f64 x,y,z;
    dvec3(){x=0,y=0,z=0;}
    dvec3(vec3 b){x=b.x,y=b.y,z=b.z;}
    dvec3(f64 _x,f64 _y,f64 _z){x=_x,y=_y,z=_z;}
};

struct uvec3{
    u32 x,y,z;
};

struct ivec3{
    i32 x,y,z;
};

struct vec4{
    union{
        struct{f32 x,y,z,w;};
        f32 data[4];
    };
    vec4(){x=0,y=0,z=0,w=0;}
    vec4(f32 _x,f32 _y,f32 _z,f32 _w){x=_x,y=_y,z=_z,w=_w;}
    vec4(vec3 v,f32 _w){x=v.x,y=v.y,z=v.z,w=_w;}
};

struct dvec4{
    union{
        struct{f64 x,y,z,w;};
        f64 data[4];
    };
    dvec4(){x=0,y=0,z=0,w=0;}
    dvec4(f64 _x,f64 _y,f64 _z,f64 _w){x=_x,y=_y,z=_z,w=_w;}
};

struct ivec4{
    i32 x,y,z,w;
};

struct uvec4{
    u32 x,y,z,w;
};

struct mat4{
    vec4 col[4];

    mat4& set_col(int idx,vec4 c){
        col[idx]=c;
        return *this;
    }
    mat4& set_row(int idx,vec4 r){
        ((f32*)&col[0])[idx]=r.x;
        ((f32*)&col[1])[idx]=r.y;
        ((f32*)&col[2])[idx]=r.z;
        ((f32*)&col[3])[idx]=r.w;
        return *this;
    }
};

struct dmat4{
    dvec4 col[4];
    dmat4& set_col(int idx,dvec4 c){
        col[idx]=c;
        return *this;
    }
    dmat4& set_row(int idx,dvec4 r){
        ((f64*)&col[0])[idx]=r.x;
        ((f64*)&col[1])[idx]=r.y;
        ((f64*)&col[2])[idx]=r.z;
        ((f64*)&col[3])[idx]=r.w;
        return *this;
    }
};