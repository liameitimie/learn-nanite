#version 450
#extension  GL_ARB_separate_shader_objects:enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location=0) out vec3 color;
layout(location=1) out vec3 coord;
layout(location=2) out float is_ext;

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

uint cycle3(uint i){
    uint imod3=i%3;
    return i-imod3+((1<<imod3)&3);
}
uint cycle3(uint i,uint ofs){
    return i-i%3+(i+ofs)%3;
}

struct FrameContext{
	mat4 vp_mat;
    uint view_mode;// 0:tri 1:cluster 2:group
    uint level;
	uint display_ext_edge;
};

FrameContext get_frame_context(){
	uint idx=push_constants.swapchain_idx;
	FrameContext context;
	for(int i=0;i<4;i++){
		vec4 p;
		p.x=uintBitsToFloat(data[idx].data[i*4]);
		p.y=uintBitsToFloat(data[idx].data[i*4+1]);
		p.z=uintBitsToFloat(data[idx].data[i*4+2]);
		p.w=uintBitsToFloat(data[idx].data[i*4+3]);
		context.vp_mat[i]=p;
	}
	context.view_mode=data[idx].data[16];
	context.level=data[idx].data[17];
	context.display_ext_edge=data[idx].data[18];
	return context;
}

struct Cluster{
	uint num_tri;
	uint idx_ofs;
	uint num_vert;
	uint pos_ofs;

	vec4 bounds;
	uint group_id;
	uint mip_level;

	//de
	uint cluster_id;
	uint buffer_idx;
	uint flag_ofs;
};

uint cluster_buffer_offset(){
	return push_constants.num_swapchain_image;
}

Cluster get_cluster(uint cluster_id){
	Cluster cluster;
	uint idx=cluster_id+cluster_buffer_offset();

	cluster.num_tri=data[nonuniformEXT(idx)].data[0];
	cluster.num_vert=data[nonuniformEXT(idx)].data[1];
	cluster.group_id=data[nonuniformEXT(idx)].data[2];
	cluster.mip_level=data[nonuniformEXT(idx)].data[3];

	cluster.bounds.x=uintBitsToFloat(data[nonuniformEXT(idx)].data[4]);
	cluster.bounds.y=uintBitsToFloat(data[nonuniformEXT(idx)].data[5]);
	cluster.bounds.z=uintBitsToFloat(data[nonuniformEXT(idx)].data[6]);
	cluster.bounds.w=uintBitsToFloat(data[nonuniformEXT(idx)].data[7]);

	cluster.idx_ofs=8;
	cluster.pos_ofs=8+cluster.num_tri;

	//de
	cluster.cluster_id=cluster_id;
	cluster.buffer_idx=idx;
	cluster.flag_ofs=8+cluster.num_tri+cluster.num_vert*3;
	return cluster;
}

vec3 get_position(Cluster cluster,uint index){
	uint tri_id=index/3;
	uint packed_tri=data[nonuniformEXT(cluster.buffer_idx)].data[tri_id+cluster.idx_ofs];
	uint v_idx=((packed_tri>>(index%3*8))&255);

	vec3 p;
    p.x=uintBitsToFloat(data[nonuniformEXT(cluster.buffer_idx)].data[v_idx*3+cluster.pos_ofs]);
    p.y=uintBitsToFloat(data[nonuniformEXT(cluster.buffer_idx)].data[v_idx*3+1+cluster.pos_ofs]);
    p.z=uintBitsToFloat(data[nonuniformEXT(cluster.buffer_idx)].data[v_idx*3+2+cluster.pos_ofs]);
	return p;
}

#define pi 3.1415926535

void main(){
	uint cluster_id=gl_InstanceIndex;
	uint index_id=gl_VertexIndex;
	uint tri_id=index_id/3;

	FrameContext context=get_frame_context();
	Cluster cluster=get_cluster(gl_InstanceIndex);

	if(tri_id>=cluster.num_tri||cluster.mip_level!=context.level){
		gl_Position.z=0/0; //discard
		return;
	}
	else{
		vec3 p;
		if(context.view_mode==3){
			if(tri_id>=64){
				gl_Position.z=0/0;
				return;
			}
			uint t=tri_id/2;
			uint x=t>>2;
			uint y=t&3;
			uint i=index_id%6;
			if(i>=3) i-=2;
			x+=(i&1);
			y+=(i>>1);
			vec3 tp;
			float theta=y*pi*0.25;
			float phi=x*pi*0.25;
			tp.x=sin(theta)*cos(phi);
			tp.y=sin(theta)*sin(phi);
			tp.z=cos(theta);

			p=cluster.bounds.xyz+tp*cluster.bounds.w;
			context.display_ext_edge=0;
		}
		else{
			p=get_position(cluster,index_id);
		}

		if(context.view_mode==0) color=to_color(tri_id);
		else if(context.view_mode==1) color=to_color(cluster_id);
		else if(context.view_mode==2) color=to_color(cluster.group_id);
		else if(context.view_mode==3) color=to_color(cluster_id);

		if(context.display_ext_edge!=0){
			uint t=data[nonuniformEXT(cluster.buffer_idx)].data[index_id+cluster.flag_ofs];
			t|=data[nonuniformEXT(cluster.buffer_idx)].data[cycle3(index_id,2)+cluster.flag_ofs];
			is_ext=t;

			vec3 tc=vec3(0);
			tc[index_id%3]=1;
			coord=tc;
		}
		else{
			is_ext=0;
			coord=vec3(0);
		}

		vec4 pos=context.vp_mat*vec4(p,1);
		pos=pos/pos.w;
		if(pos.z<0||pos.z>1) pos.z=0/0;
		gl_Position=pos;
	}
}
