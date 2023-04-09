#version 450
#extension  GL_ARB_separate_shader_objects:enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location=0) out vec3 color;

layout(set=0,binding=0) buffer BindlessData{
    uint data[];
}data[];

layout(push_constant) uniform constant{
	uint swapchain_idx;
	uint num_swapchain_image;
}push_constants;

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
	uint Hash = MurmurMix(idx+1);

	vec3 color=vec3(
		(Hash>>0)&255,
		(Hash>>8)&255,
		(Hash>>16)&255
	);

	return color*(1.0f/255.0f);
}


void main(){
	uint c_ofs=push_constants.num_swapchain_image;
	
    uint c_id=gl_InstanceIndex+c_ofs;
    uint tri_id=gl_VertexIndex/3;

    uint num_tri=data[nonuniformEXT(c_id)].data[0];
    uint num_vert=data[nonuniformEXT(c_id)].data[1];
	uint group_id=data[nonuniformEXT(c_id)].data[2];

	uint t_ofs=4;
	uint v_ofs=4+num_tri;

    if(tri_id>=num_tri) return;

    uint packed_tri=data[nonuniformEXT(c_id)].data[tri_id+t_ofs];
    uint v_idx=((packed_tri>>(gl_VertexIndex%3*8))&255);

    vec4 p;
    p.x=uintBitsToFloat(data[nonuniformEXT(c_id)].data[v_idx*3+v_ofs]);
    p.y=uintBitsToFloat(data[nonuniformEXT(c_id)].data[v_idx*3+1+v_ofs]);
    p.z=uintBitsToFloat(data[nonuniformEXT(c_id)].data[v_idx*3+2+v_ofs]);
    p.w=1;

	mat4 vp_mat;
	uint idx=push_constants.swapchain_idx;
	for(int i=0;i<4;i++){
		vec4 p;
		p.x=uintBitsToFloat(data[idx].data[i*4]);
		p.y=uintBitsToFloat(data[idx].data[i*4+1]);
		p.z=uintBitsToFloat(data[idx].data[i*4+2]);
		p.w=uintBitsToFloat(data[idx].data[i*4+3]);
		vp_mat[i]=p;
	}

	uint view_mode=data[idx].data[16];

    if(view_mode==0) color=to_color(tri_id);
	else if(view_mode==1) color=to_color(c_id);
	else if(view_mode==2){
		// color=(group_id==0)?vec3(1):vec3(0);
		color=to_color(group_id);
	}

    p=vp_mat*p;
    p/=p.w;

    if(p.z<0||p.z>1) p.z=0/0;

    gl_Position=p;
}
