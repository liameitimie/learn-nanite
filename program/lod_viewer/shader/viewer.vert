#version 450
#extension  GL_ARB_separate_shader_objects:enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location=0) out vec3 color;

layout(set=0,binding=0) buffer BindlessBuffer{
    uint data[];
}buffers[];

layout(push_constant) uniform constant{
	uint swapchain_idx;
}push_constants;

uint num_swapchain_image(){
    return buffers[0].data[0];
}

struct Cluster{
    uint num_vert;
    uint v_data_ofs;
    uint num_tri;
    uint t_data_ofs;

    uint group_id;
    uint mip_level;
};

struct FrameContext{
    mat4 vp_mat;
    mat4 v_mat;
    uint view_mode;// 0:tri 1:cluster 2:group 3:level
};

Cluster get_cluster(uint cluster_id){
    Cluster cluster;
    uint idx=1+3*num_swapchain_image();
    uint ofs=4+16*cluster_id;

    cluster.num_vert=buffers[idx].data[0+ofs];
    cluster.v_data_ofs=buffers[idx].data[1+ofs];
    cluster.num_tri=buffers[idx].data[2+ofs];
    cluster.t_data_ofs=buffers[idx].data[3+ofs];

    // cluster.lod_bounds.x=uintBitsToFloat(buffers[idx].data[4+ofs]);
	// cluster.lod_bounds.y=uintBitsToFloat(buffers[idx].data[5+ofs]);
	// cluster.lod_bounds.z=uintBitsToFloat(buffers[idx].data[6+ofs]);
	// cluster.lod_bounds.w=uintBitsToFloat(buffers[idx].data[7+ofs]);

    // cluster.parent_lod_bounds.x=uintBitsToFloat(buffers[idx].data[8+ofs]);
	// cluster.parent_lod_bounds.y=uintBitsToFloat(buffers[idx].data[9+ofs]);
	// cluster.parent_lod_bounds.z=uintBitsToFloat(buffers[idx].data[10+ofs]);
	// cluster.parent_lod_bounds.w=uintBitsToFloat(buffers[idx].data[11+ofs]);

    // cluster.lod_error=uintBitsToFloat(buffers[idx].data[12+ofs]);
    // cluster.max_parent_lod_error=uintBitsToFloat(buffers[idx].data[13+ofs]);

    cluster.group_id=buffers[idx].data[14+ofs];
    cluster.mip_level=buffers[idx].data[15+ofs];
    return cluster;
}

FrameContext get_frame_context(){
	uint idx=push_constants.swapchain_idx+1+2*num_swapchain_image();
	FrameContext context;
	for(int i=0;i<4;i++){
		vec4 p;
		p.x=uintBitsToFloat(buffers[idx].data[i*4]);
		p.y=uintBitsToFloat(buffers[idx].data[i*4+1]);
		p.z=uintBitsToFloat(buffers[idx].data[i*4+2]);
		p.w=uintBitsToFloat(buffers[idx].data[i*4+3]);
		context.vp_mat[i]=p;
	}
    for(int i=0;i<4;i++){
		vec4 p;
		p.x=uintBitsToFloat(buffers[idx].data[i*4+16]);
		p.y=uintBitsToFloat(buffers[idx].data[i*4+1+16]);
		p.z=uintBitsToFloat(buffers[idx].data[i*4+2+16]);
		p.w=uintBitsToFloat(buffers[idx].data[i*4+3+16]);
		context.v_mat[i]=p;
	}
    context.view_mode=buffers[idx].data[32];
	return context;
}

uint get_visiable_cluster(uint id){
    uint idx=push_constants.swapchain_idx+1+num_swapchain_image();
    return buffers[idx].data[id];
}

vec3 get_position(Cluster cluster,uint index){
    uint tri_id=index/3;
    uint idx=1+3*num_swapchain_image();
	uint packed_tri=buffers[idx].data[tri_id+cluster.t_data_ofs];
	uint v_idx=((packed_tri>>(index%3*8))&255);

	vec3 p;
    p.x=uintBitsToFloat(buffers[idx].data[v_idx*3+cluster.v_data_ofs]);
    p.y=uintBitsToFloat(buffers[idx].data[v_idx*3+1+cluster.v_data_ofs]);
    p.z=uintBitsToFloat(buffers[idx].data[v_idx*3+2+cluster.v_data_ofs]);
	return p;
}

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
	uint cluster_id=get_visiable_cluster(gl_InstanceIndex);
	uint index=gl_VertexIndex;
	uint tri_id=index/3;

    FrameContext context=get_frame_context();
	Cluster cluster=get_cluster(cluster_id);

    if(tri_id>=cluster.num_tri){
        gl_Position.z=0/0;
        return;
    }

    if(context.view_mode==0) color=to_color(tri_id);
	else if(context.view_mode==1) color=to_color(cluster_id);
	else if(context.view_mode==2) color=to_color(cluster.group_id);
	else if(context.view_mode==3) color=to_color(cluster.mip_level);

    vec3 p=get_position(cluster,index);

    // vec4 pos=context.vp_mat*vec4(p,1);
	// pos=pos/pos.w;
	// if(pos.z<0||pos.z>1) pos.z=0/0;
	// gl_Position=pos;
	gl_Position=context.vp_mat*vec4(p,1);
}