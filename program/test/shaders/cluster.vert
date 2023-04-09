#version 450
#extension  GL_ARB_separate_shader_objects:enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location=0) out vec3 color;

layout(set=0,binding=0) buffer PackedClusters{
    uint data[];
}packed_cluster[];

layout(push_constant) uniform constant{
	mat4 vp_mat;
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
	uint Hash = MurmurMix(idx);

	vec3 color=vec3(
		(Hash>>0)&255,
		(Hash>>8)&255,
		(Hash>>16)&255
	);

	return color*(1.0f/255.0f);
}

void main(){
    uint cluster_id=gl_InstanceIndex;
    uint tri_id=gl_VertexIndex/3;

    uint num_tri=packed_cluster[cluster_id].data[0];
    uint num_vert=packed_cluster[cluster_id].data[1];

    if(tri_id>=num_tri) return;

    uint packed_tri=packed_cluster[cluster_id].data[tri_id+2];
    uint v_idx=((packed_tri>>(gl_VertexIndex%3*8))&255);

    vec4 p;
    p.x=uintBitsToFloat(packed_cluster[cluster_id].data[v_idx*3+num_tri+2]);
    p.y=uintBitsToFloat(packed_cluster[cluster_id].data[v_idx*3+1+num_tri+2]);
    p.z=uintBitsToFloat(packed_cluster[cluster_id].data[v_idx*3+2+num_tri+2]);
    p.w=1;

    // color=to_color(tri_id);
    color=to_color(cluster_id);

    p=push_constants.vp_mat*p;
    p/=p.w;

    if(p.z<0||p.z>1) p.z=0/0;

    gl_Position=p;
}