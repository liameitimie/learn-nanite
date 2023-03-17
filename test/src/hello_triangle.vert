#version 450
#extension  GL_ARB_separate_shader_objects:enable

layout(location=0) out vec2 p;

vec2 pos[3]={vec2(-1,-1),vec2(1,-1),vec2(-1,1)};
//vec3 col[3]={vec3(1,0,0),vec3(0,1,0),vec3(0,0,1)};

void main(){
    //color=col[gl_VertexIndex];
    p=pos[gl_VertexIndex];
    gl_Position=vec4(pos[gl_VertexIndex],0,1);
}

//glslangValidator -V -o hello_triangle_vert.spv hello_triangle.vert