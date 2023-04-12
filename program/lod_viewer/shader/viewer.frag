#version 450
#extension  GL_ARB_separate_shader_objects:enable

layout(location=0) in vec3 color;
layout(location=0) out vec4 out_color;

void main(){
    vec3 c=mix(color,vec3(1),0.2);
    out_color=vec4(c,1);
    // out_color=vec4(color,1);
}
