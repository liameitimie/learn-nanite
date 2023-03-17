#pragma once
#include <math.h>

#define pi 3.14159265358979323846

typedef unsigned long long u64;
typedef int i32;
typedef unsigned int u32;
typedef float f32;
typedef double f64;

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
        ((float*)&col[0])[idx]=r.x;
        ((float*)&col[1])[idx]=r.y;
        ((float*)&col[2])[idx]=r.z;
        ((float*)&col[3])[idx]=r.w;
        return *this;
    }
};

inline mat4 operator+(mat4 a,mat4 b){
    mat4 c;
    for(int i=0;i<16;i++) ((float*)&c)[i]=((float*)&a)[i]+((float*)&b)[i];
    return c;
}
inline mat4& operator+=(mat4 &a,mat4 b){
    for(int i=0;i<16;i++) ((float*)&a)[i]+=((float*)&b)[i];
    return a;
}

inline mat4 mul(mat4 a,mat4 b){
    mat4 c{};
    for(int j=0;j<4;j++){
        for(int k=0;k<4;k++){
            f32 t=b.col[j].data[k];
            for(int i=0;i<4;i++){
                c.col[j].data[i]+=t*a.col[k].data[i];
            }
        }
    }
    return c;
}

inline vec4 mul(mat4 a,vec4 b){
    vec4 c{};
    for(int k=0;k<4;k++){
        for(int i=0;i<4;i++){
            c.data[i]+=b.data[k]*a.col[k].data[i];
        }
    }
    return c;
}

// http://rodolphe-vaillant.fr/entry/7/c-code-for-4x4-matrix-inversion
inline bool invertColumnMajor(const float m[16], float invOut[16])
{
    float inv[16], det;
    int i;
 
    inv[ 0] =  m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    inv[ 4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    inv[ 8] =  m[4] * m[ 9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[ 9];
    inv[12] = -m[4] * m[ 9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[ 9];
    inv[ 1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    inv[ 5] =  m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    inv[ 9] = -m[0] * m[ 9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[ 9];
    inv[13] =  m[0] * m[ 9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[ 9];
    inv[ 2] =  m[1] * m[ 6] * m[15] - m[1] * m[ 7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[ 7] - m[13] * m[3] * m[ 6];
    inv[ 6] = -m[0] * m[ 6] * m[15] + m[0] * m[ 7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[ 7] + m[12] * m[3] * m[ 6];
    inv[10] =  m[0] * m[ 5] * m[15] - m[0] * m[ 7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[ 7] - m[12] * m[3] * m[ 5];
    inv[14] = -m[0] * m[ 5] * m[14] + m[0] * m[ 6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[ 6] + m[12] * m[2] * m[ 5];
    inv[ 3] = -m[1] * m[ 6] * m[11] + m[1] * m[ 7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[ 9] * m[2] * m[ 7] + m[ 9] * m[3] * m[ 6];
    inv[ 7] =  m[0] * m[ 6] * m[11] - m[0] * m[ 7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[ 8] * m[2] * m[ 7] - m[ 8] * m[3] * m[ 6];
    inv[11] = -m[0] * m[ 5] * m[11] + m[0] * m[ 7] * m[ 9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[ 9] - m[ 8] * m[1] * m[ 7] + m[ 8] * m[3] * m[ 5];
    inv[15] =  m[0] * m[ 5] * m[10] - m[0] * m[ 6] * m[ 9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[ 9] + m[ 8] * m[1] * m[ 6] - m[ 8] * m[2] * m[ 5];
 
    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
 
    if(det == 0) return false;
 
    det = 1.f / det;
 
    for(i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;
 
    return true;
}

inline bool invert(mat4 m,mat4& inv){
    return invertColumnMajor((f32*)&m,(f32*)&inv);
}

inline vec3 operator*(vec3 a,f32 b){
    return vec3{a.x*b,a.y*b,a.z*b};
}
inline vec3 operator+(vec3 a,vec3 b){
    return vec3{a.x+b.x,a.y+b.y,a.z+b.z};
}
inline vec3 operator-(vec3 a,vec3 b){
    return vec3{a.x-b.x,a.y-b.y,a.z-b.z};
}
inline vec3& operator+=(vec3& a,vec3 b){
    a.x+=b.x,a.y+=b.y,a.z+=b.z;
    return a;
}


inline vec3 operator-(vec3 a){return {-a.x,-a.y,-a.z};}

inline vec3 cross(vec3 a,vec3 b){
    return vec3{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};
}

inline vec3 normalize(vec3 a){
    f32 rl=1/sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
    return a*rl;
}

inline f32 dot(vec3 a,vec3 b){
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

inline f32 dot(vec4 a,vec4 b){
    return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;
}

inline vec4 operator*(vec4 a,f32 b){
    return vec4{a.x*b,a.y*b,a.z*b,a.w*b};
}