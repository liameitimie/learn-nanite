#version 450
#extension  GL_ARB_separate_shader_objects:enable

layout(location=0) in vec3 c;
layout(location=0) out vec4 out_color;

void main(){
    //float c=(p.x+1)/2;
    //c*=2;
    //c=pow(c,2.2);
    //out_color=vec4(c,c,c,1);
    // vec3 color=clamp(c,vec3(0),vec3(1));
    out_color=vec4(c,1);
}
