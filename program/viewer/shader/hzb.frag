#version 450
#extension  GL_ARB_separate_shader_objects:enable
#extension GL_EXT_nonuniform_qualifier : enable

// layout(location=0) in vec2 uv;
layout(set=1,binding=0) uniform sampler2D level_deps[];

layout(push_constant) uniform constant{
	// uvec2 lst_mip_size;
    uint lst_level;
}pc;

ivec2 mip1_to_mip0(ivec2 p){
    return p*ivec2(1920,1080)/ivec2(1024,1024);
}

void main(){
    ivec2 p=ivec2(gl_FragCoord);

    if(pc.lst_level==0) p=mip1_to_mip0(p);
    else p=2*p;

    float x=texture(level_deps[pc.lst_level],p+0.5).x;
    float y=texture(level_deps[pc.lst_level],p+0.5+ivec2(1,0)).x;
    float z=texture(level_deps[pc.lst_level],p+0.5+ivec2(1,1)).x;
    float w=texture(level_deps[pc.lst_level],p+0.5+ivec2(0,1)).x;

    float min_z=min(min(x,y),min(z,w));
    gl_FragDepth=min_z;
}

// void main(){
//     ivec2 texel_idx=2*ivec2(gl_FragCoord);

//     float x=texture(level_deps[pc.lst_level],texel_idx+0.5).x;
//     float y=texture(level_deps[pc.lst_level],texel_idx+0.5+ivec2(1,0)).x;
//     float z=texture(level_deps[pc.lst_level],texel_idx+0.5+ivec2(1,1)).x;
//     float w=texture(level_deps[pc.lst_level],texel_idx+0.5+ivec2(0,1)).x;

//     float min_z=min(min(x,y),min(z,w));

//     bool ext_x=(pc.lst_mip_size.x&1)!=0&&texel_idx.x==pc.lst_mip_size.x-3;
//     bool ext_y=(pc.lst_mip_size.y&1)!=0&&texel_idx.y==pc.lst_mip_size.y-3;

//     if(ext_x){
//         float ext1=texture(level_deps[pc.lst_level],texel_idx+0.5+ivec2(2,0)).x;
//         float ext2=texture(level_deps[pc.lst_level],texel_idx+0.5+ivec2(2,1)).x;
//         min_z=min(min_z,min(ext1,ext2));
//         if(ext_y){
//             float ext3=texture(level_deps[pc.lst_level],texel_idx+0.5+ivec2(2,2)).x;
//             min_z=min(min_z,ext3);
//         }
//     }
//     if(ext_y){
//         float ext1=texture(level_deps[pc.lst_level],texel_idx+0.5+ivec2(0,2)).x;
//         float ext2=texture(level_deps[pc.lst_level],texel_idx+0.5+ivec2(1,2)).x;
//         min_z=min(min_z,min(ext1,ext2));
//     }
//     gl_FragDepth=min_z;
// }