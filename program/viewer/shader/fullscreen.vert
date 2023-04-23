#version 450
#extension  GL_ARB_separate_shader_objects:enable

// layout(location=0) out vec2 uv;

vec3 p[]={
    {-1,-1,0},
    {-1,1,0},
    {1,-1,0},
    {1,1,0}
};

// vec2 uvs[]={
//     {0,0},
//     {0,1},
//     {1,0},
//     {1,1}
// };

void main(){
    int id=gl_VertexIndex;
    if(id>=3) id-=2;
    // uv=uvs[id];
    gl_Position=vec4(p[id],1);
}