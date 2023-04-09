#version 450
#extension  GL_ARB_separate_shader_objects:enable

layout(location=0) in vec3 color;
layout(location=1) in vec3 coord;
layout(location=2) in float is_ext;
layout(location=0) out vec4 out_color;

void main(){
    float t=1-min(min(coord.x,coord.y),coord.z);
    t=min(t,is_ext);
    t=clamp(pow(t+0.1,50)+0.3,0,1);

    vec3 c=mix(color,vec3(1),t);
    out_color=vec4(c,1);
}
