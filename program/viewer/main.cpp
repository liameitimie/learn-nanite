#include <cmath>
#include <vk.h>
#include <window.h>
#include <mesh.h>
#include <mesh_simplify.h>
#include <virtual_mesh.h>
#include "camera.h"
#include <fmt/core.h>
#include <timer.h>
#include <assert.h>
#include <fmt/os.h>
#include <co/fs.h>
#include <algorithm>

using namespace fmt;
using namespace vk;
using namespace vk_win;

const u32 width=1920;
const u32 height=1080;

u32 high_bit(u32 x){
    u32 res=0,t=16,y=0;
    y=-((x>>t)!=0),res+=y&t,x>>=y&t,t>>=1;
    y=-((x>>t)!=0),res+=y&t,x>>=y&t,t>>=1;
    y=-((x>>t)!=0),res+=y&t,x>>=y&t,t>>=1;
    y=-((x>>t)!=0),res+=y&t,x>>=y&t,t>>=1;
    y=(x>>t)!=0,res+=y;
    return res;
}

// u32 high_bit1(u32 x){
//     u32 res=0,t=16;
//     while(t){
//         if((x>>t)!=0) 
//             res+=t,x>>=t;
//         t>>=1;
//     }
//     return res;
// }

u32 as_uint(f32 x){
    return *((u32*)&x);
};
f32 as_float(u32 x){
    return *((f32*)&x);
}

void packing_virtual_mesh(const VirtualMesh& vm,vector<u32>& packed_data){
    // vector<u32> packed_data;
    packed_data.clear();

    packed_data.push_back(vm.clusters.size()); //num cluster
    packed_data.push_back(vm.cluster_groups.size()); //num group
    packed_data.push_back(0); //group data ofs
    packed_data.push_back(0);
    for(auto& cluster:vm.clusters){
        packed_data.push_back(cluster.verts.size()); //num vert
        packed_data.push_back(0); //v data ofs
        packed_data.push_back(cluster.indexes.size()/3); //num tri
        packed_data.push_back(0); //t data ofs

        packed_data.push_back(as_uint(cluster.sphere_bounds.center.x)); //bounds
        packed_data.push_back(as_uint(cluster.sphere_bounds.center.y));
        packed_data.push_back(as_uint(cluster.sphere_bounds.center.z));
        packed_data.push_back(as_uint(cluster.sphere_bounds.radius));

        packed_data.push_back(as_uint(cluster.lod_bounds.center.x)); //lod bounds
        packed_data.push_back(as_uint(cluster.lod_bounds.center.y));
        packed_data.push_back(as_uint(cluster.lod_bounds.center.z));
        packed_data.push_back(as_uint(cluster.lod_bounds.radius));

        Sphere parent_lod_bounds=vm.cluster_groups[cluster.group_id].lod_bounds;
        f32 max_parent_lod_error=vm.cluster_groups[cluster.group_id].max_parent_lod_error;
        packed_data.push_back(as_uint(parent_lod_bounds.center.x)); //parent lod bounds
        packed_data.push_back(as_uint(parent_lod_bounds.center.y));
        packed_data.push_back(as_uint(parent_lod_bounds.center.z));
        packed_data.push_back(as_uint(parent_lod_bounds.radius));

        packed_data.push_back(as_uint(cluster.lod_error));
        packed_data.push_back(as_uint(max_parent_lod_error));
        packed_data.push_back(cluster.group_id);
        packed_data.push_back(cluster.mip_level);
    }

    packed_data[2]=packed_data.size(); //group data ofs
    for(auto& group:vm.cluster_groups){
        packed_data.push_back(group.clusters.size()); //num cluster
        packed_data.push_back(0); //cluter data ofs
        packed_data.push_back(as_uint(group.max_parent_lod_error));
        packed_data.push_back(0);

        packed_data.push_back(as_uint(group.lod_bounds.center.x)); //lod bounds
        packed_data.push_back(as_uint(group.lod_bounds.center.y));
        packed_data.push_back(as_uint(group.lod_bounds.center.z));
        packed_data.push_back(as_uint(group.lod_bounds.radius));
    }
    u32 i=0;
    for(auto& cluster:vm.clusters){
        u32 ofs=4+20*i;
        packed_data[ofs+1]=packed_data.size();
        for(vec3 p:cluster.verts){
            packed_data.push_back(as_uint(p.x));
            packed_data.push_back(as_uint(p.y));
            packed_data.push_back(as_uint(p.z));
        }

        packed_data[ofs+3]=packed_data.size();
        for(u32 i=0;i<cluster.indexes.size()/3;i++){ //tri data
            u32 i0=cluster.indexes[i*3];
            u32 i1=cluster.indexes[i*3+1];
            u32 i2=cluster.indexes[i*3+2];
            assert(i0<256&&i1<256&&i2<256);

            u32 packed_tri=(i0|(i1<<8)|(i2<<16));
            packed_data.push_back(packed_tri);
        }
        i++;
    }
    i=0;
    for(auto& group:vm.cluster_groups){
        u32 ofs=packed_data[2]+8*i;
        packed_data[ofs+1]=packed_data.size();
        for(u32 x:group.clusters){
            packed_data.push_back(x);
        }
        i++;
    }
}

// void packing_group_data(const VirtualMesh& vm,vector<u32>& packed_group_data){
//     packed_group_data.clear();
//     packed_group_data.push_back(vm.cluster_groups.size()); //num group
//     packed_group_data.push_back(0);
//     packed_group_data.push_back(0);
//     packed_group_data.push_back(0);
//     for(auto& group:vm.cluster_groups){
//         packed_group_data.push_back(group.clusters.size()); //num cluster
//         packed_group_data.push_back(0); //cluter data ofs
//         packed_group_data.push_back(as_uint(group.max_parent_lod_error));
//         packed_group_data.push_back(0);

//         packed_group_data.push_back(as_uint(group.lod_bounds.center.x)); //lod bounds
//         packed_group_data.push_back(as_uint(group.lod_bounds.center.y));
//         packed_group_data.push_back(as_uint(group.lod_bounds.center.z));
//         packed_group_data.push_back(as_uint(group.lod_bounds.radius));
//     }
//     u32 i=0;
//     for(auto& group:vm.cluster_groups){
//         u32 ofs=4+8*i;
//         packed_group_data[ofs+1]=packed_group_data.size();
//         for(u32 x:group.clusters){
//             packed_group_data.push_back(x);
//         }
//         i++;
//     }
// }

struct ConstContext{
    u32 num_swapchain_image;
};

struct FrameContext{
    mat4 vp_mat;
    mat4 v_mat;
    mat4 p_mat;
    mat4 vp_mat2;
    u32 view_mode;
};

Timer timer;

u32 num_clusters=0;
u32 num_groups=0;
vector<u32> packed_data;
// vector<u32> packed_group_data;

int main(){
#if 0
    timer.reset();
    print("loading mesh: ");
    Mesh mesh;
    mesh.load("../asset/Font_Reconstructed.stl");
    print("{} us\n",timer.us());

    VirtualMesh vm;
    vm.build(mesh);

    // Bounds box=vm.clusters.back().box_bounds;
    // print("pmin:({},{},{}), pmax:({},{},{})\n",box.pmin.x,box.pmin.y,box.pmin.z,box.pmax.x,box.pmax.y,box.pmax.z);

    timer.reset();
    print("packing virtual mesh: ");
    packing_virtual_mesh(vm,packed_data);
    print("size: {} bytes, ",packed_data.size()*4);
    print("time: {} us\n",timer.us());

    // timer.reset();
    // print("packing group data: ");
    // packing_group_data(vm,packed_group_data);
    // print("size: {} bytes, ",packed_group_data.size()*4);
    // print("time: {} us\n",timer.us());

    timer.reset();
    print("writting packed data: ");
    fs::file file("packed_data.txt",'w');
    file.write(packed_data.data(),packed_data.size()*sizeof(u32));
    print("{} us\n",timer.us());

    return 0;
#else
    fs::file file("packed_data.txt",'r');
    if(!file){
        print("failed: packed_data.txt file invaild");
        return 0;
    }
    packed_data.resize(file.size()/sizeof(u32));

    timer.reset();
    print("loading packed data: ");
    file.read(packed_data.data(),file.size());
    print("{} us\n",timer.us());

    num_clusters=packed_data[0];
    num_groups=packed_data[1];


    // print("num clusters: {}\n",num_clusters);

    // Camera camera{
    //     .position=vec3{0,0,10},
    //     .pitch=0,
    //     .yaw=-90,
    // };

    // mat4 v_mat=camera.view_mat();
    // mat4 p_mat=camera.projection_mat(Camera::radians(40),(f32)width/height);
    // vec3 p=normalize(vec3{0,p_mat.col[1].data[1],1});
    // f32 d=dot(p,vec3{0,0,-1});
    // mat4 vp_mat=mul(p_mat,v_mat);

    // tCluster cluster=get_cluster(20000);
    
    // Bounds clip_bounds,p_bounds;
    // for(u32 i=0;i<cluster.num_vert;i++){
    //     vec3 p=get_position(cluster,i);
    //     p_bounds=p_bounds+p;
    //     vec4 clip_p=mul(vp_mat,vec4(p,1));
    //     clip_p=clip_p*(1/clip_p.w);
    //     print("{}, p:({},{},{}), clip_p:({},{},{})\n",i,p.x,p.y,p.z,clip_p.x,clip_p.y,clip_p.z);
    //     clip_bounds=clip_bounds+vec3{clip_p.x,clip_p.y,clip_p.z};
    // }
    // print("pmin:({},{},{}), pmax:({},{},{})\n",p_bounds.pmin.x,p_bounds.pmin.y,p_bounds.pmin.z,p_bounds.pmax.x,p_bounds.pmax.y,p_bounds.pmax.z);
    // print("clip pmin:({},{},{}), pmax:({},{},{})\n",clip_bounds.pmin.x,clip_bounds.pmin.y,clip_bounds.pmin.z,clip_bounds.pmax.x,clip_bounds.pmax.y,clip_bounds.pmax.z);

    // print("bounds center:({},{},{}), radius:{}\n",cluster.bounds.x,cluster.bounds.y,cluster.bounds.z,cluster.bounds.w);

    // vec4 sphere=cluster.bounds;
    // vec4 view_c=mul(v_mat,vec4{sphere.x,sphere.y,sphere.z,1});
    // vec4 view_rect=sphere_to_screen_rect(vec3{view_c.x,view_c.y,view_c.z},sphere.w,p_mat);
    // print("rect pmin:({},{}), pmax:({},{})\n",view_rect.x,view_rect.y,view_rect.z,view_rect.w);

    // f32 nz=view_c.z+sphere.w;
    // nz=-0.1/nz;
    // print("nz:{}\n",nz);

    // for(u32 i=0;i<num_clusters;i++){
    //     tCluster cluster=get_cluster(i);
    //     Bounds clip_bounds;
    //     for(u32 j=0;j<cluster.num_vert;j++){
    //         vec3 p=get_position(cluster,j);
    //         vec4 clip_p=mul(vp_mat,vec4(p,1));
    //         clip_p=clip_p*(1/clip_p.w);
    //         clip_bounds=clip_bounds+vec3{clip_p.x,clip_p.y,clip_p.z};
    //     }
    //     vec4 sphere=cluster.bounds;
    //     vec4 view_c=mul(v_mat,vec4{sphere.x,sphere.y,sphere.z,1});
    //     vec4 view_rect=sphere_to_screen_rect(vec3{view_c.x,view_c.y,view_c.z},sphere.w,p_mat);
    //     f32 nz=view_c.z+sphere.w;
    //     nz=-0.1/nz;
        
    //     assert(clip_bounds.pmin.x>=view_rect.x);
    //     assert(clip_bounds.pmin.y>=view_rect.y);
    //     assert(clip_bounds.pmax.x<=view_rect.z);
    //     assert(clip_bounds.pmax.y<=view_rect.w);
    //     assert(clip_bounds.pmax.z<=nz);
    // }

    vk::init();
    Window window=Window::create(width,height,"virtual mesh viewer");
    window.build_vk_surface(vk::instance());
    vk::init_surface(window.surface,window.width,window.height);

    auto packed_buffer=Buffer::from_iter(
        BufferAllocateInfo{
            .buffer_usage=vk::BufferUsage::StorageBuffer,
            .memory_usage=vk::MemoryUsage::Upload
        },
        packed_data
    )
    .unwrap();

    vector<ConstContext> const_contexts={{vk::num_swapchain_image()}};
    Buffer const_context_buffer=Buffer::from_iter(
        BufferAllocateInfo{
            .buffer_usage=BufferUsage::StorageBuffer,
            .memory_usage=MemoryUsage::Upload
        },
        const_contexts
    ).unwrap();

    vector<Buffer> frame_context_buffers(vk::num_swapchain_image());
    for(auto& buffer:frame_context_buffers){
        buffer=Buffer::from_raw(
            BufferAllocateInfo{
                .buffer_usage=BufferUsage::StorageBuffer,
                .memory_usage=MemoryUsage::Upload
            },
            nullptr,
            sizeof(FrameContext),
            0
        ).unwrap();
    }

    // vector<u32> id(vk::num_swapchain_image());
    // for(u32 i=0;i<vk::num_swapchain_image();i++) id[i]=i;

    vector<Buffer> indirect_buffer(vk::num_swapchain_image());
    for(auto& buffer:indirect_buffer){
        buffer=Buffer::from_raw(
            BufferAllocateInfo{
                .buffer_usage=BufferUsage::StorageBuffer|BufferUsage::IndirectBuffer,
                .memory_usage=MemoryUsage::Download,
            },
            nullptr,
            4*sizeof(u32),
            0
        ).unwrap();
    }

    vector<Buffer> visiable_clusters_buffer(vk::num_swapchain_image());
    for(auto& buffer:visiable_clusters_buffer){
        buffer=Buffer::from_raw(
            BufferAllocateInfo{
                .buffer_usage=BufferUsage::StorageBuffer,
                // .memory_usage=MemoryUsage::Download,
                .memory_usage=MemoryUsage::GpuOnly
            },
            nullptr,
            (1<<22),
            0
        ).unwrap();
    }

    vector<vec3> instance_data;
    for(u32 i=0;i<15;i++){
        for(u32 j=0;j<15;j++){
            for(u32 k=0;k<4;k++)
                instance_data.push_back({i*5.0f,k*7.0f,j*5.0f});
        }
    }
    auto instance_buffer=Buffer::from_iter(
        BufferAllocateInfo{
            .buffer_usage=vk::BufferUsage::StorageBuffer,
            .memory_usage=vk::MemoryUsage::Upload
        },
        instance_data
    )
    .unwrap();

    vk::write_bindless_set(
        0,
        const_context_buffer
    );
    vk::write_bindless_set(
        1,
        indirect_buffer.data(),
        indirect_buffer.size()
    );
    vk::write_bindless_set(
        1+vk::num_swapchain_image(),
        visiable_clusters_buffer.data(),
        visiable_clusters_buffer.size()
    );
    vk::write_bindless_set(
        1+2*vk::num_swapchain_image(),
        frame_context_buffers.data(),
        frame_context_buffers.size()
    );
    vk::write_bindless_set(
        1+3*vk::num_swapchain_image(),
        packed_buffer
    );
    vk::write_bindless_set(
        2+3*vk::num_swapchain_image(),
        instance_buffer
    );

    // u32 mip_levels=high_bit(std::max(width,height))+1;
    // u32 mip_levels=high_bit(2048)+1;
    auto dep_img=vk::create_image2d(ImageDesc2D{
        .width=width,
        .height=height,
        // .width=2048,
        // .height=2048,
        .format=Format::D32_SFLOAT,
        .usage=ImageUsage::DepthStencilAttachment|ImageUsage::Sampled,
        .mip_levels=1,
    }).unwrap();

    auto view2_img=vk::create_image2d(ImageDesc2D{
        .width=width,
        .height=height,
        .format=Format::B8G8R8A8_UNORM,
        .usage=ImageUsage::ColorAttachment|ImageUsage::TransferSrc,
        .mip_levels=1
    }).unwrap();

    u32 hzb_mip_levels=high_bit(1024)+1;
    auto hzb_img=vk::create_image2d(ImageDesc2D{
        .width=1024,
        .height=1024,
        .format=Format::D32_SFLOAT,
        .usage=ImageUsage::DepthStencilAttachment|ImageUsage::Sampled,
        .mip_levels=hzb_mip_levels
    }).unwrap();

    vector<ImageView> dep_level_view(hzb_mip_levels+1);
    u32 i=0;
    for(auto& view:dep_level_view){
        if(i==0){
            view=dep_img.view(ImageViewDesc{
                .format=dep_img.format,
                .mip_level=0,
                .level_count=1
            }).unwrap();
        }
        else{
            view=hzb_img.view(ImageViewDesc{
                .format=hzb_img.format,
                .mip_level=i-1,
                .level_count=1
            }).unwrap();
        }
        i++;
    }

    Sampler unnorm_sampler=vk::create_sampler(SamplerDesc{.unnorm_coord=true}).unwrap();
    Sampler hzb_sampler=vk::create_sampler(SamplerDesc{.unnorm_coord=false}).unwrap();

    vector<CombinedImageSampler> img_sampler;
    for(auto& v:dep_level_view){
        img_sampler.push_back({&unnorm_sampler,&v});
    }

    vk::write_bindless_set(0,img_sampler.data(),img_sampler.size());

    ImageView hzb_view=hzb_img.view(ImageViewDesc{
        .format=hzb_img.format,
        .mip_level=0,
        .level_count=hzb_img.mip_levels
    }).unwrap();

    vk::write_bindless_set(img_sampler.size(),CombinedImageSampler{&hzb_sampler,&hzb_view});

    auto g_ppl=GraphicsPipeline::new_()
        .render_pass(RenderingCreateInfo{
            .color_attachment_formats={vk::swapchain_image_format()},
            .depth_attachment_format=Format::D32_SFLOAT
        })
        .push_constant_size(8)
        .input_assembly_state(PrimitiveTopology::TriangleList)
        .vertex_shader(ShaderModule::from_file("shader/viewer_vert.spv").unwrap())
        // .viewport_state(ViewportState::Default({Viewport::dimension(width,height)}))
        .depth_stencil_state(DepthStencilState::reverse_z_test())
        .fragment_shader(ShaderModule::from_file("shader/viewer_frag.spv").unwrap())
        .dynamic_state({DynamicState::Viewport})
        .build().unwrap();

    auto c_ppl=ComputePipeline::new_()
        .push_constant_size(4)
        .compute_shader(ShaderModule::from_file("shader/culling_comp.spv").unwrap())
        .build().unwrap();
    
    auto hzb_ppl=GraphicsPipeline::new_()
        .render_pass(RenderingCreateInfo{
            .depth_attachment_format=Format::D32_SFLOAT
        })
        .push_constant_size(4)
        .input_assembly_state(PrimitiveTopology::TriangleList)
        .vertex_shader(ShaderModule::from_file("shader/fullscreen_vert.spv").unwrap())
        .depth_stencil_state(DepthStencilState::always())
        .fragment_shader(ShaderModule::from_file("shader/hzb_frag.spv").unwrap())
        .dynamic_state({DynamicState::Viewport})
        .build().unwrap();

    auto cmd_allocator=CommandBufferAllocator::new_().unwrap();
    vector<CommandBuffer> cmds(vk::num_swapchain_image());
    u32 swapchain_idx=0;

    for(auto& cmd:cmds){
        u32 ctx[2]={swapchain_idx,0};
        cmd=CommandBuffer::new_(cmd_allocator).unwrap();
        cmd.begin(CommandBufferUsage::SimultaneousUse).unwrap()
            .bind_descriptor_sets(PipelineBindPoint::Compute,c_ppl.pipeline_layout,0,vk::bindless_buffer_set())
            .bind_descriptor_sets(PipelineBindPoint::Compute,c_ppl.pipeline_layout,1,vk::bindless_image_set())
            .push_constant(c_ppl.pipeline_layout,4,&swapchain_idx)
            .pipeline_barrier(Dependency{
                .buffer_barriers={BufferBarrier{
                    .buffer=indirect_buffer[swapchain_idx],
                    .dst_stage=PipelineStage::COMPUTE_SHADER,
                    .dst_access=AccessFlag::SHADER_WRITE
                }},
                // .image_barriers={ImageBarrier{
                //     .image=hzb_img,
                //     .src_stage=PipelineStage::LATE_FRAGMENT_TESTS,
                //     .dst_stage=PipelineStage::COMPUTE_SHADER,
                //     .dst_access=AccessFlag::SHADER_READ,
                //     // .new_layout=ImageLayout::ShaderReadOnlyOptimal,
                // }}
            })
            .bind_compute_pipeline(c_ppl.handle)
            .dispatch(num_groups*instance_data.size()/32+1,1,1)
            // .dispatch(num_clusters*instance_data.size()/32+1,1,1)
            .pipeline_barrier(Dependency{
                .buffer_barriers={BufferBarrier{
                    .buffer=indirect_buffer[swapchain_idx],
                    .dst_stage=PipelineStage::DRAW_INDIRECT,
                    .dst_access=AccessFlag::INDIRECT_COMMAND_READ
                }},
                .image_barriers={
                    ImageBarrier{
                        .image=vk::swapchain_image(swapchain_idx),
                        .dst_stage=PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                        .dst_access=AccessFlag::COLOR_ATTACHMENT_WRITE,
                        .new_layout=ImageLayout::AttachmentOptimal,
                    },
                    ImageBarrier{
                        .image=dep_img,
                        .dst_stage=PipelineStage::EARLY_FRAGMENT_TESTS,
                        .dst_access=AccessFlag::DEPTH_STENCIL_ATTACHMENT_WRITE,
                        .new_layout=ImageLayout::AttachmentOptimal,
                    }
                }
            })
            .begin_rendering(RenderingInfo{
                .render_area_extent={.x=window.width,.y=window.height},
                .color_attachments={RenderingAttachmentInfo{
                    .image_view=swapchain_image(swapchain_idx).image_view,
                    .load_op=LoadOp::Clear,
                }},
                .depth_attachment=Some(RenderingAttachmentInfo{
                    .image_view=dep_level_view[0].handle,
                    .load_op=LoadOp::Clear,
                })
            })
            .push_constant(g_ppl.pipeline_layout,8,ctx)
            .bind_descriptor_sets(PipelineBindPoint::Graphics,g_ppl.pipeline_layout,0,vk::bindless_buffer_set())
            .bind_graphics_pipeline(g_ppl.handle)
            .set_viewport({Viewport::dimension(width,height)})
            .draw_indirect(indirect_buffer[swapchain_idx])
            .end_rendering()
            .pipeline_barrier(Dependency{
                .image_barriers={
                    ImageBarrier{
                        .image=vk::swapchain_image(swapchain_idx),
                        .src_stage=PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                        .src_access=AccessFlag::COLOR_ATTACHMENT_WRITE,
                        .dst_stage=PipelineStage::TRANSFER,
                        .dst_access=AccessFlag::TRANSFER_WRITE,
                        .old_layout=ImageLayout::AttachmentOptimal,
                        // .new_layout=ImageLayout::PresentSrc
                        .new_layout=ImageLayout::TransferDstOptimal
                    },
                    ImageBarrier{
                        .image=dep_img,
                        .src_stage=PipelineStage::LATE_FRAGMENT_TESTS,
                        .src_access=AccessFlag::DEPTH_STENCIL_ATTACHMENT_WRITE,
                        .dst_stage=PipelineStage::FRAGMENT_SHADER,
                        .dst_access=AccessFlag::SHADER_READ,
                        .old_layout=ImageLayout::AttachmentOptimal,
                        .new_layout=ImageLayout::ShaderReadOnlyOptimal,
                    }
                }
            });

        cmd.bind_descriptor_sets(PipelineBindPoint::Graphics,hzb_ppl.pipeline_layout,1,vk::bindless_image_set());

        // u32 cw=2048,ch=2048;
        // u32 cw=width,ch=height;
        for(u32 i=0,cw=1024,ch=1024;i<hzb_mip_levels;i++,cw>>=1,ch>>=1){
            cmd.pipeline_barrier(Dependency{
                    .image_barriers={ImageBarrier{
                        .image=hzb_img,
                        .dst_stage=PipelineStage::EARLY_FRAGMENT_TESTS,
                        .dst_access=AccessFlag::DEPTH_STENCIL_ATTACHMENT_WRITE,
                        .new_layout=ImageLayout::AttachmentOptimal,
                        .sub_range=Some(ImageSubRange{
                            .base_mip_level=i,
                            .level_count=1
                        })
                    }}
                })
                .begin_rendering(RenderingInfo{
                    .render_area_extent={.x=cw,.y=ch},
                    .depth_attachment=Some(RenderingAttachmentInfo{
                        .image_view=dep_level_view[i+1].handle,
                        .load_op=LoadOp::Clear,
                    })
                })
                .bind_graphics_pipeline(hzb_ppl.handle)
                .set_viewport({Viewport::dimension(cw,ch)})
                .push_constant(hzb_ppl.pipeline_layout,4,&i)
                .draw(6,1,0,0)
                .end_rendering()
                .pipeline_barrier(Dependency{
                    .image_barriers={ImageBarrier{
                        .image=hzb_img,
                        .src_stage=PipelineStage::LATE_FRAGMENT_TESTS,
                        .src_access=AccessFlag::DEPTH_STENCIL_ATTACHMENT_WRITE,
                        .dst_stage=PipelineStage::FRAGMENT_SHADER,
                        .dst_access=AccessFlag::SHADER_READ,
                        .old_layout=ImageLayout::AttachmentOptimal,
                        .new_layout=ImageLayout::ShaderReadOnlyOptimal,
                        .sub_range=Some(ImageSubRange{
                            .base_mip_level=i,
                            .level_count=1
                        })
                    }}
                });
            
        }
        cmd.pipeline_barrier(Dependency{
            .image_barriers={ImageBarrier{
                .image=hzb_img,
            }}
        });
        ctx[1]=1;
        cmd.pipeline_barrier(Dependency{
                .image_barriers={
                    ImageBarrier{
                        .image=view2_img,
                        .dst_stage=PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                        .dst_access=AccessFlag::COLOR_ATTACHMENT_WRITE,
                        .new_layout=ImageLayout::AttachmentOptimal,
                    },
                    ImageBarrier{
                        .image=dep_img,
                        .src_stage=PipelineStage::FRAGMENT_SHADER,
                        .src_access=AccessFlag::SHADER_READ,
                        .dst_stage=PipelineStage::EARLY_FRAGMENT_TESTS,
                        .dst_access=AccessFlag::DEPTH_STENCIL_ATTACHMENT_WRITE,
                        .old_layout=ImageLayout::ShaderReadOnlyOptimal,
                        .new_layout=ImageLayout::AttachmentOptimal,
                    }
                }
            })
            .begin_rendering(RenderingInfo{
                .render_area_extent={.x=window.width,.y=window.height},
                .color_attachments={RenderingAttachmentInfo{
                    .image_view=view2_img.image_view,
                    .load_op=LoadOp::Clear,
                }},
                .depth_attachment=Some(RenderingAttachmentInfo{
                    .image_view=dep_level_view[0].handle,
                    .load_op=LoadOp::Clear,
                })
            })
            .push_constant(g_ppl.pipeline_layout,8,ctx)
            .bind_descriptor_sets(PipelineBindPoint::Graphics,g_ppl.pipeline_layout,0,vk::bindless_buffer_set())
            .bind_graphics_pipeline(g_ppl.handle)
            .set_viewport({Viewport::dimension(width,height)})
            .draw_indirect(indirect_buffer[swapchain_idx])
            .end_rendering()
            .pipeline_barrier(Dependency{
                .image_barriers={
                    ImageBarrier{
                        .image=view2_img,
                        .src_stage=PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                        .src_access=AccessFlag::COLOR_ATTACHMENT_WRITE,
                        .dst_stage=PipelineStage::TRANSFER,
                        .dst_access=AccessFlag::TRANSFER_READ,
                        .old_layout=ImageLayout::AttachmentOptimal,
                        .new_layout=ImageLayout::TransferSrcOptimal
                    },
                }
            })
            .blit_image(
                view2_img,
                vk::swapchain_image(swapchain_idx),
                ivec4{0,0,width,height},
                ivec4{0,height*3/4,width/4,height}
            )
            .pipeline_barrier(Dependency{
                .image_barriers={
                    ImageBarrier{
                        .image=vk::swapchain_image(swapchain_idx),
                        .src_stage=PipelineStage::TRANSFER,
                        .src_access=AccessFlag::TRANSFER_WRITE,
                        .old_layout=ImageLayout::TransferDstOptimal,
                        .new_layout=ImageLayout::PresentSrc
                    },
                }
            });

        cmd.build().unwrap();
        
        swapchain_idx++;
    }

    struct FrameSync{
        Semaphore acq_img=vk::create_semaphore().unwrap();
        Semaphore cmd_exec=vk::create_semaphore().unwrap();
        Fence cpu_wait_cmd=vk::create_fence(true).unwrap();
    }sync[3];
    u32 frame_idx=0,frame_cnt=0;

    Camera camera{
        .position=vec3{0,0,-10},
        .pitch=0,
        .yaw=90,
    };

    Camera camera2{
        .position=vec3{50,100,50},
        .pitch=-89,
        .yaw=0,
    };

    window.set_cursor_disabled();
    bool is_cursor_disabled=true;
    dvec2 lst_cursor_pos;
    u32 view_mode=0;

    timer.reset();

    auto out=output_file("debug.txt");

    while(!window.should_close()){
        window.poll_events();

        f32 tick_time=timer.us()/1000.f;
        timer.reset();

        if(window.is_key_down('W')) camera.move_front(tick_time);
        if(window.is_key_down('S')) camera.move_front(-tick_time);
        if(window.is_key_down('A')) camera.move_right(-tick_time);
        if(window.is_key_down('D')) camera.move_right(tick_time);

        if(window.is_key_begin_press('J')) view_mode=(view_mode+4-1)%4;
        if(window.is_key_begin_press('K')) view_mode=(view_mode+1)%4;

        if(window.is_key_begin_press('B')){
            window.set_cursor_normal();
            lst_cursor_pos=window.get_cursor_pos();
            is_cursor_disabled=false;
        }
        if(window.is_key_begin_release('B')){
            window.set_cursor_disabled();
            is_cursor_disabled=true;
        }

        dvec2 cursor_pos=window.get_cursor_pos();
        if(frame_cnt==0) lst_cursor_pos=cursor_pos;
        if(is_cursor_disabled)
            camera.rotate_view(cursor_pos.x-lst_cursor_pos.x,cursor_pos.y-lst_cursor_pos.y);
        lst_cursor_pos=cursor_pos;

        mat4 v_mat=camera.view_mat();
        mat4 p_mat=camera.projection_mat(Camera::radians(40),(f32)window.width/window.height);
        mat4 vp_mat=mul(p_mat,v_mat);

        mat4 v_mat2=camera2.view_mat();
        mat4 p_mat2=camera2.projection_mat(Camera::radians(40),(f32)window.width/window.height);
        mat4 vp_mat2=mul(p_mat2,v_mat2);

        vk::acquire_next_image(Some(sync[frame_idx].acq_img),None(),swapchain_idx);
        vk::wait_for_fence(sync[frame_idx].cpu_wait_cmd);
        vk::reset_fence(sync[frame_idx].cpu_wait_cmd);

        FrameContext frame_context{vp_mat,v_mat,p_mat,vp_mat2,view_mode};
        frame_context_buffers[swapchain_idx].update(&frame_context,sizeof(FrameContext));

        vector<u32> tmp(4);
        vk::read_from_buffer(tmp.data(),indirect_buffer[swapchain_idx],sizeof(u32)*4);
        out.print("frame: {}, visable cluster: {}\n",frame_cnt,tmp[1]);

        u32 c[4]={128*3,0,0,0};
        indirect_buffer[swapchain_idx].update(c,sizeof(u32)*4);

        vk::queue_submit(
            SubmitInfo{
                .waiting={sync[frame_idx].acq_img},
                .command_buffers={cmds[swapchain_idx]},
                .signal={sync[frame_idx].cmd_exec}
            },
            sync[frame_idx].cpu_wait_cmd
        );
        vk::queue_present(PresentInfo{
            .waiting={sync[frame_idx].cmd_exec},
            .swapchain_image_idx=swapchain_idx
        });

        frame_idx=(frame_idx+1)%3;
        frame_cnt++;

        // break;
    }
    vk::cleanup();
#endif
    return 0;
}