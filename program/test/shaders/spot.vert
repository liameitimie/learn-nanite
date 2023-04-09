#version 450
#extension  GL_ARB_separate_shader_objects:enable

layout(location=0) in vec3 pos;

layout(location=0) out vec3 c;

layout(push_constant) uniform constant{
	mat4 vp_mat;
	//vec4 data;
}push_constants;

// vec2 pos[3]={vec2(-1,-1),vec2(1,-1),vec2(-1,1)};
//vec3 col[3]={vec3(1,0,0),vec3(0,1,0),vec3(0,0,1)};

uint MurmurMix(uint Hash){
	Hash^=Hash>>16;
	Hash*=0x85ebca6b;
	Hash^=Hash>>13;
	Hash*=0xc2b2ae35;
	Hash^=Hash>>16;
	return Hash;
}

vec3 to_color(uint idx)
{
	uint Hash = MurmurMix(idx);

	vec3 color=vec3(
		(Hash>>0)&255,
		(Hash>>8)&255,
		(Hash>>16)&255
	);

	return color*(1.0f/255.0f);
}


void main(){
    //color=col[gl_VertexIndex];
    uint tri_id=gl_VertexIndex/3;
    c=to_color(tri_id);
	//mat4 t=push_constants.vp_mat*3;
    //gl_Position=vec4(pos,1);
	vec4 p=push_constants.vp_mat*vec4(pos,1);
	p/=p.w;
	if(p.z<0||p.z>1) p.z=0/0;
	gl_Position=p;
}
