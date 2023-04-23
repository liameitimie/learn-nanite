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

using namespace fmt;
using namespace vk;
using namespace vk_win;

const u32 width=1920;
const u32 height=1080;

u32 as_uint(f32 x){
    return *((u32*)&x);
};

void packing_virtual_mesh(const VirtualMesh& vm,vector<u32>& packed_data){
    // vector<u32> packed_data;
    packed_data.clear();

    packed_data.push_back(vm.clusters.size()); //num cluster
    packed_data.push_back(0);
    packed_data.push_back(0);
    packed_data.push_back(0);
    for(auto& cluster:vm.clusters){
        packed_data.push_back(cluster.verts.size()); //num vert
        packed_data.push_back(0); //v data ofs
        packed_data.push_back(cluster.indexes.size()/3); //num tri
        packed_data.push_back(0); //t data ofs

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
    u32 i=0;
    for(auto& cluster:vm.clusters){
        u32 ofs=4+16*i;
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
    // print("packed data size: {} bytes\n",packed_data.size()*4);

    // auto packed_buffer=Buffer::from_iter(
    //     BufferAllocateInfo{
    //         .buffer_usage=vk::BufferUsage::StorageBuffer,
    //         .memory_usage=vk::MemoryUsage::Upload
    //     },
    //     packed_data
    // )
    // .unwrap();

    // return packed_buffer;
}

struct ConstContext{
    u32 num_swapchain_image;
};

struct FrameContext{
    mat4 vp_mat;
    mat4 v_mat;
    u32 view_mode;
};

int main(){
    Timer timer;

    u32 num_clusters=0;
    vector<u32> packed_data;

#if 0
    timer.reset();
    print("loading mesh: ");
    Mesh mesh;
    mesh.load("../asset/Font_Reconstructed.stl");
    print("{} us\n",timer.us());

    VirtualMesh vm;
    vm.build(mesh);

    timer.reset();
    print("packing virtual mesh: ");
    packing_virtual_mesh(vm,packed_data);
    print("size: {} bytes, ",packed_data.size()*4);
    print("time: {} us\n",timer.us());

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
#endif

    vk::init();
    Window window=Window::create(width,height,"virtual mesh viewer");
    window.build_vk_surface(vk::instance());
    vk::init_surface(window.surface,window.width,window.height);

    // Buffer pack_buffer=packing_virtual_mesh(vm);
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

    vector<u32> id(vk::num_swapchain_image());
    for(u32 i=0;i<vk::num_swapchain_image();i++) id[i]=i;

    vector<Buffer> indirect_buffer(vk::num_swapchain_image());
    for(auto& buffer:indirect_buffer){
        buffer=Buffer::from_raw(
            BufferAllocateInfo{
                .buffer_usage=BufferUsage::StorageBuffer|BufferUsage::IndirectBuffer,
                .memory_usage=MemoryUsage::Upload,
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
                .memory_usage=MemoryUsage::Download,
                // .memory_usage=MemoryUsage::GpuOnly
            },
            nullptr,
            (1<<20),
            0
        ).unwrap();
    }

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

    auto g_ppl=GraphicsPipeline::new_()
        .render_pass(RenderingCreateInfo{
            .color_attachment_formats={vk::swapchain_image_format()},
            .depth_attachment_format=Format::D32_SFLOAT
        })
        .push_constant_size(4)
        .input_assembly_state(PrimitiveTopology::TriangleList)
        .vertex_shader(ShaderModule::from_file("shader/viewer_vert.spv").unwrap())
        .viewport_state(ViewportState::Default({Viewport::dimension(width,height)}))
        .depth_stencil_state(DepthStencilState::reverse_z_test())
        .fragment_shader(ShaderModule::from_file("shader/viewer_frag.spv").unwrap())
        .build().unwrap();

    auto c_ppl=ComputePipeline::new_()
        .push_constant_size(4)
        .compute_shader(ShaderModule::from_file("shader/culling_comp.spv").unwrap())
        .build().unwrap();
    
    auto cmd_allocator=CommandBufferAllocator::new_().unwrap();
    vector<CommandBuffer> cmds(vk::num_swapchain_image());

    u32 swapchain_idx=0;

    auto depth_buffer=Image::AttachmentImage(
        width,
        height,
        Format::D32_SFLOAT,
        ImageUsage::DepthStencilAttachment
    ).unwrap();

    for(auto& cmd:cmds){
        cmd=CommandBuffer::new_(cmd_allocator).unwrap()
            .begin(CommandBufferUsage::SimultaneousUse).unwrap()
            .bind_descriptor_sets(PipelineBindPoint::Compute,c_ppl.pipeline_layout,0,vk::bindless_buffer_set())
            .push_constant(c_ppl.pipeline_layout,4,&id[swapchain_idx])
            .pipeline_barrier(Dependency{
                .buffer_barriers={BufferBarrier{
                    .buffer=indirect_buffer[swapchain_idx],
                    .dst_stage=PipelineStage::COMPUTE_SHADER,
                    .dst_access=AccessFlag::SHADER_WRITE
                }}
            })
            .bind_compute_pipeline(c_ppl.handle)
            .dispatch(num_clusters/32+1,1,1)
            .pipeline_barrier(Dependency{
                .buffer_barriers={BufferBarrier{
                    .buffer=indirect_buffer[swapchain_idx],
                    .dst_stage=PipelineStage::DRAW_INDIRECT,
                    .dst_access=AccessFlag::INDIRECT_COMMAND_READ
                }},
                .image_barriers={ImageBarrier{
                    .image=vk::swapchain_image(swapchain_idx),
                    .dst_stage=PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                    .dst_access=AccessFlag::COLOR_ATTACHMENT_WRITE,
                    .new_layout=ImageLayout::AttachmentOptimal,
                }}
            })
            .begin_rendering(RenderingInfo{
                .render_area_extent={.x=window.width,.y=window.height},
                .color_attachments={RenderingAttachmentInfo{
                    .image_view=swapchain_image(swapchain_idx).image_view,
                    .load_op=LoadOp::Clear,
                }},
                .depth_attachment=Some(RenderingAttachmentInfo{
                    .image_view=depth_buffer.image_view,
                    .load_op=LoadOp::Clear,
                })
            })
            .bind_descriptor_sets(PipelineBindPoint::Graphics,g_ppl.pipeline_layout,0,vk::bindless_buffer_set())
            .bind_graphics_pipeline(g_ppl.handle)
            .draw_indirect(indirect_buffer[swapchain_idx])
            .end_rendering()
            .pipeline_barrier(Dependency{
                .image_barriers={ImageBarrier{
                    .image=vk::swapchain_image(swapchain_idx),
                    .src_stage=PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                    .src_access=AccessFlag::COLOR_ATTACHMENT_WRITE,
                    .old_layout=ImageLayout::AttachmentOptimal,
                    .new_layout=ImageLayout::PresentSrc
                }}
            })
            .build().unwrap();

        swapchain_idx++;
    }

    struct FrameSync{
        Semaphore acq_img=vk::create_semaphore().unwrap();
        Semaphore cmd_exec=vk::create_semaphore().unwrap();
        Fence cpu_wait_cmd=vk::create_fence(true).unwrap();
    }sync[3];
    u32 frame_idx=0,frame_cnt=0;

    Camera camera;
    camera.position=vec3{0,0,10};
    camera.yaw=-90;
    camera.pitch=0;

    window.set_cursor_disabled();
    bool is_cursor_disabled=true;
    dvec2 lst_cursor_pos;
    u32 view_mode=0;

    timer.reset();

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

        vk::acquire_next_image(Some(sync[frame_idx].acq_img),None(),swapchain_idx);
        vk::wait_for_fence(sync[frame_idx].cpu_wait_cmd);
        vk::reset_fence(sync[frame_idx].cpu_wait_cmd);

        FrameContext frame_context{vp_mat,v_mat,view_mode};
        frame_context_buffers[swapchain_idx].update(&frame_context,sizeof(FrameContext));

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

    // mat4 v_mat=camera.view_mat();
    // mat4 p_mat=camera.projection_mat(Camera::radians(40),(f32)window.width/window.height);
    // mat4 vp_mat=mul(p_mat,v_mat);

    // swapchain_idx=0;

    // FrameContext frame_context{vp_mat,v_mat};
    // frame_context_buffers[swapchain_idx].update(&frame_context,sizeof(FrameContext));

    // auto cmd=CommandBuffer::new_(cmd_allocator).unwrap()
    //     .begin(CommandBufferUsage::SimultaneousUse).unwrap()
    //     .bind_descriptor_sets(PipelineBindPoint::Compute,c_ppl.pipeline_layout,vk::bindless_set())
    //     .push_constant(c_ppl.pipeline_layout,4,&id[0])
    //     .bind_compute_pipeline(c_ppl.handle)
    //     .dispatch(vm.clusters.size()/32+1,1,1)
    //     .build().unwrap();
    
    // auto fence=vk::create_fence(false).unwrap();
    // vk::queue_submit(
    //     SubmitInfo{.command_buffers={cmds[swapchain_idx]}},
    //     fence
    // );
    // vk::wait_for_fence(fence);

    // vector<u32> tmp(1<<20);
    // vk::read_from_buffer(tmp.data(),visiable_clusters_buffer[0],1<<20);

    // timer.reset();
    // auto out=output_file("debug.txt");
    // out.print("cnt: {}\n",tmp[0]);
    // for(u32 i=0;i<tmp[0];i++){
    //     out.print("{}\n",tmp[i+1]);
    // }
    // print("print file: {} us\n",timer.us());

    return 0;
}