#version 450
#extension  GL_ARB_separate_shader_objects:enable

layout(location=0) in vec2 pos;
layout(location=1) in vec3 col;

layout(location=0) out vec3 c;

// vec2 pos[3]={vec2(-1,-1),vec2(1,-1),vec2(-1,1)};
//vec3 col[3]={vec3(1,0,0),vec3(0,1,0),vec3(0,0,1)};

void main(){
    //color=col[gl_VertexIndex];
    c=col;
    gl_Position=vec4(pos,0,1);
}

//glslangValidator -V -o hello_triangle_vert.spv hello_triangle.vert