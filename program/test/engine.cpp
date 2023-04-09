#include "engine.h"
#include <vk.h>
#include <iostream>
#include <chrono>
#include <vector>

// #define GLFW_INCLUDE_VULKAN
// #include "GLFW/glfw3.h"

#include "camera.h"
#include <mesh.h>
#include <mesh_simplify.h>
#include <vec_math.h>
#include <cluster.h>
#include <assert.h>
#include <mesh_util.h>

using namespace std;

void Engine::init(){
    vk::init();
    window=vk_win::Window::create(1920,1080,"learn nanite");
    window.build_vk_surface(vk::instance());
    vk::init_surface(window.surface,window.width,window.height);
}

using namespace std::chrono;
class Timer{
public:
    Timer(){
        reset();
    }
    void reset(){
        start = high_resolution_clock::now();
    }
    long long us(){
    //当前时钟减去开始时钟的count
        return duration_cast<microseconds>(high_resolution_clock::now() - start).count();
    }
private:
    time_point<high_resolution_clock> start;
};


void Engine::run(){
    Timer timer;

    timer.reset();
    Mesh mesh;
    // mesh.positions={{0,0,0},{1,0,0},{0,1,0},{1,1,0},{0,0,1},{1,0,1},{0,1,1},{1,1,1}};
    // mesh.indices={0,1,2,1,2,3, 0,1,4,1,4,5, 0,2,4,2,4,6, 1,3,5,3,5,7, 2,3,6,3,6,7, 4,5,7,4,7,6};
    // mesh.load("asset/spot_triangulated_good.obj");
    mesh.load("asset/Aset_wood_root_M_rkswd_LOD0.fbx");
    cout<<"load mesh: "<<timer.us()<<" us"<<endl;

    auto&[pos,idx]=mesh;
    // vector<Cluster> clusters;
    // timer.reset();
    // cluster_triangles(pos,idx,clusters);
    // cout<<"cluster triangles: "<<timer.us()<<" us"<<endl;

    //Mesh mesh1{clusters[0].verts,clusters[0].indexes};
    // Mesh mesh1;

    // timer.reset();
    // for(auto& cluster:clusters){
    //     auto& pos=cluster.verts;
    //     auto& idx=cluster.indexes;
    //     MeshSimplifier simplifier(pos.data(),pos.size(),idx.data(),idx.size());
        
    //     for(u32 e:cluster.external_edges){
    //         simplifier.lock_position(pos[idx[e]]);
    //         simplifier.lock_position(pos[idx[cycle3(e)]]);
    //     }

    //     simplifier.simplify(Cluster::cluster_size/2);
    //     pos.resize(simplifier.remaining_num_vert());
    //     idx.resize(simplifier.remaining_num_tri()*3);
    // }
    // cout<<"simplify clusters: "<<timer.us()<<" us"<<endl;

    // vector<vk::Buffer> packed_clusters;
    // packed_clusters.reserve(clusters.size());

    // for(const auto& cluster:clusters){
    //     vector<u32> packed_cluster;
    //     packed_cluster.push_back(cluster.indexes.size()/3); // num_tri
    //     packed_cluster.push_back(cluster.verts.size()); // num_vert
    //     for(u32 i=0;i<cluster.indexes.size()/3;i++){
    //         u32 i0=cluster.indexes[i*3];
    //         u32 i1=cluster.indexes[i*3+1];
    //         u32 i2=cluster.indexes[i*3+2];
    //         assert(i0<256&&i1<256&&i2<256);

    //         u32 packed_tri=(i0|(i1<<8)|(i2<<16));
    //         packed_cluster.push_back(packed_tri);
    //     }
    //     auto as_uint=[](f32 x)->u32{
    //         return *((u32*)&x);
    //     };
    //     for(vec3 p:cluster.verts){
    //         packed_cluster.push_back(as_uint(p.x));
    //         packed_cluster.push_back(as_uint(p.y));
    //         packed_cluster.push_back(as_uint(p.z));
    //     }
    //     auto cluster_buffer=vk::Buffer::from_iter(
    //         vk::BufferAllocateInfo{
    //             .buffer_usage=vk::BufferUsage::StorageBuffer,
    //             .memory_usage=vk::MemoryUsage::Upload
    //         },
    //         packed_cluster
    //     )
    //     .unwrap();
    //     packed_clusters.push_back(cluster_buffer);
    // }

    // vk::write_bindless_set(0,packed_clusters.data(),packed_clusters.size(),vk::DescriptorType::StorageBuffer);

    // timer.reset();
    // MeshSimplifier simplifier(pos.data(),pos.size(),idx.data(),idx.size());
    // simplifier.simplify(5000);
    // cout<<"simplify: "<<timer.us()<<" us"<<endl;

    // pos.resize(simplifier.remaining_num_vert());
    // idx.resize(simplifier.remaining_num_tri()*3);

    // cout<<"max error: "<<simplifier.max_error()<<endl;

    // while(idx.size()/3>2000){
    //     MeshSimplifier simplifier(pos.data(),pos.size(),idx.data(),idx.size());
    //     simplifier.simplify(idx.size()/3-16);

    //     cout<<simplifier.remaining_num_vert()<<' '<<simplifier.remaining_num_tri()<<endl;
    //     pos.resize(simplifier.remaining_num_vert());
    //     idx.resize(simplifier.remaining_num_tri()*3);
    // }

    vector<vec3> vertices=mesh.flatten();
    // vector<vec3> vertices=mesh1.positions;

    auto vertex_buffer=vk::Buffer::from_iter(
        vk::BufferAllocateInfo{
            .buffer_usage=vk::BufferUsage::VertexBuffer,
            .memory_usage=vk::MemoryUsage::Upload
        },
        vertices
    )
    .unwrap();

    // auto index_buffer=vk::Buffer::from_iter(
    //     vk::BufferAllocateInfo{
    //         .buffer_usage=vk::BufferUsage::IndexBuffer,
    //         .memory_usage=vk::MemoryUsage::Upload
    //     },
    //     mesh.indices
    // )
    // .unwrap();

    auto depth_buffer=vk::Image::AttachmentImage(
        window.width,
        window.height,
        vk::Format::D32_SFLOAT,
        vk::ImageUsage::DepthStencilAttachment
    ).unwrap();

    auto pipeline=vk::GraphicsPipeline::new_()
        .render_pass(vk::RenderingCreateInfo{
            .color_attachment_formats=vector{vk::swapchain_image_format()},
            .depth_attachment_format=vk::Format::D32_SFLOAT
        })
        .push_constant_size(64)
        .vertex_input_state(Mesh::vertex_desc())
        .input_assembly_state((vk::InputAssemblyState::new_()))
        .vertex_shader(vk::ShaderModule::from_file("shaders/spot_vert.spv").unwrap())
        .viewport_state(vk::ViewportState::viewport_fixed_scissor_irrelevant(vector{vk::Viewport::dimension(window.width,window.height)}))
        .depth_stencil_state(vk::DepthStencilState::reverse_z_test())
        .fragment_shader(vk::ShaderModule::from_file("shaders/spot_frag.spv").unwrap())
        .build().unwrap();

    auto cmd_allocator=vk::CommandBufferAllocator::new_().unwrap();
    vector<vk::CommandBuffer> cmds;
    for(int i=0;i<vk::num_swapchain_image();i++) cmds.push_back(vk::CommandBuffer::new_(cmd_allocator).unwrap());

    u32 swapchain_idx=0;

    struct FrameSync{
        vk::Semaphore acq_img=vk::create_semaphore().unwrap();
        vk::Semaphore cmd_exec=vk::create_semaphore().unwrap();
        vk::Fence cpu_wait_cmd=vk::create_fence(true).unwrap();
    }sync[3];
    u32 frame_idx=0;

    
    uint64_t sum_time=0,frame_cnt=0;
    uint64_t min_times[3]={(uint64_t)(-1),(uint64_t)(-1),(uint64_t)(-1)};
    uint64_t max_times[3]={0,0,0};

    Camera camera;
    camera.position=vec3{0,0,10};
    camera.yaw=-90;
    camera.pitch=0;

    window.set_cursor_disabled();
    dvec2 lst_cursor_pos;

    timer.reset();
    while(!window.should_close()){
        f32 tick_time=timer.us()/1000.f;
        timer.reset();

        window.poll_events();

        if(window.is_key_down('W')) camera.move_front(tick_time);
        if(window.is_key_down('S')) camera.move_front(-tick_time);
        if(window.is_key_down('A')) camera.move_right(-tick_time);
        if(window.is_key_down('D')) camera.move_right(tick_time);

        if(window.is_key_down('B')) window.set_cursor_normal(),lst_cursor_pos=window.get_cursor_pos();

        //cout<<tick_time<<endl;

        dvec2 cursor_pos=window.get_cursor_pos();
        if(frame_cnt==0) lst_cursor_pos=cursor_pos;
        camera.rotate_view(cursor_pos.x-lst_cursor_pos.x,cursor_pos.y-lst_cursor_pos.y);
        lst_cursor_pos=cursor_pos;
        
        mat4 v_mat=camera.view_mat();
        mat4 p_mat=camera.projection_mat(Camera::radians(40),(f32)window.width/window.height);
        mat4 vp_mat=mul(p_mat,v_mat);

        // auto print=[](vec4 v){
        //     cout<<v.x<<' '<<v.y<<' '<<v.z<<' '<<v.w<<endl;
        // };

        // vec4 v1=mul(vp_mat,vec4(1,0,0,1));
        // vec4 v2=mul(vp_mat,vec4(0,1,0,1));
        // vec4 v3=mul(vp_mat,vec4(1,1,0,1));
        // vec4 v4=mul(vp_mat,vec4(0,0,0,1));

        // print(v1);
        // print(v2);
        // print(v3);
        // print(v4);

        vk::acquire_next_image(Some(sync[frame_idx].acq_img),None(),swapchain_idx);
        vk::wait_for_fence(sync[frame_idx].cpu_wait_cmd);
        vk::reset_fence(sync[frame_idx].cpu_wait_cmd);

        auto cmd=cmds[swapchain_idx];

        cmd.reset().unwrap()
            .begin(vk::CommandBufferUsage::OneTimeSubmit).unwrap()
            .pipeline_barrier(vk::Dependency{
                .image_barriers=vector{vk::ImageBarrier{
                    .image=vk::swapchain_image(swapchain_idx),
                    .dst_stage=vk::PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                    .dst_access=vk::AccessFlag::COLOR_ATTACHMENT_WRITE,
                    .new_layout=vk::ImageLayout::AttachmentOptimal,
                }}
            })
            .begin_rendering(vk::RenderingInfo{
                .render_area_extent={.x=window.width,.y=window.height},
                .color_attachments=vector{vk::RenderingAttachmentInfo{
                    .image_view=vk::swapchain_image(swapchain_idx).image_view,
                    .load_op=vk::LoadOp::Clear,
                }},
                .depth_attachment=Some(vk::RenderingAttachmentInfo{
                    .image_view=depth_buffer.image_view,
                    .load_op=vk::LoadOp::Clear,
                })
            })
            .push_constant(pipeline.pipeline_layout,64,&vp_mat)
            .bind_graphics_pipeline(pipeline.handle)
            .bind_descriptor_sets(vk::PipelineBindPoint::Graphics,pipeline.pipeline_layout,vk::bindless_set())
            // .draw(128*3,clusters.size(),0,0)
            .bind_vertex_buffer(vector{vertex_buffer})
            .draw(vertices.size(),1,0,0)
            // .bind_index_buffer(index_buffer)
            // .draw_indexed(mesh.indices.size(),1,0,0,0)
            .end_rendering()
            .pipeline_barrier(vk::Dependency{
                .image_barriers=vector{vk::ImageBarrier{
                    .image=vk::swapchain_image(swapchain_idx),
                    .src_stage=vk::PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                    .src_access=vk::AccessFlag::COLOR_ATTACHMENT_WRITE,
                    .old_layout=vk::ImageLayout::AttachmentOptimal,
                    .new_layout=vk::ImageLayout::PresentSrc
                }}
            })
            .build().unwrap();

        vk::queue_submit(vk::SubmitInfo{
            .waiting=vector{sync[frame_idx].acq_img},
            //.command_buffers=vector{cmds[swapchain_idx]},
            .command_buffers=vector{cmd},
            .signal=vector{sync[frame_idx].cmd_exec}
        },
        sync[frame_idx].cpu_wait_cmd);

        vk::queue_present(vk::PresentInfo{
            .waiting=vector{sync[frame_idx].cmd_exec},
            .swapchain_image_idx=swapchain_idx});

        frame_idx=(frame_idx+1)%3;

        uint64_t frame_time=timer.us();
        // std::cout<<frame_time/1000.f<<"\n";
        sum_time+=frame_time;
        frame_cnt++;

        //vk::cleanup();

        if(frame_time<min_times[2]){
            min_times[2]=frame_time;
            if(min_times[2]<min_times[1]){
                std::swap(min_times[1],min_times[2]);
                if(min_times[1]<min_times[0]) std::swap(min_times[0],min_times[1]);
            }
        }

        if(frame_time>max_times[2]){
            max_times[2]=frame_time;
            if(max_times[2]>max_times[1]){
                std::swap(max_times[1],max_times[2]);
                if(max_times[1]>max_times[0]) std::swap(max_times[0],max_times[1]);
            }
        }
    }

    std::cout<<"frame cnt:"<<frame_cnt<<", averge frame time:"<<sum_time/1000.f/frame_cnt<<std::endl;
    std::cout<<"min times:"<<min_times[0]/1000.f<<' '<<min_times[1]/1000.f<<' '<<min_times[2]/1000.f<<std::endl;
    std::cout<<"max times:"<<max_times[0]/1000.f<<' '<<max_times[1]/1000.f<<' '<<max_times[2]/1000.f<<std::endl;
}

void Engine::cleanup(){
    vk::cleanup();
}