#include "engine.h"
#include "vk/vk.h"
#include <iostream>

using namespace std;

void Engine::init(){
    vk::init(1920,1080);
}

void Engine::run(){
    // struct Vertex{
    //     vec2 pos;
    //     vec3 col;
    //     static auto per_vertex()->vk::VertexBufferDescription{
    //         vector member{
    //             vk::VertexMemberInfo{
    //                 .offset=0,
    //                 .format=vk::Format::R32G32_SFLOAT
    //             },
    //             vk::VertexMemberInfo{
    //                 .offset=8,
    //                 .format=vk::Format::R32G32B32_SFLOAT
    //             }
    //         };
    //         return vk::VertexBufferDescription{
    //             .member=member,
    //             .stride=sizeof(Vertex),
    //             .input_rate=vk::VertexInputRate::Vertex
    //         };
    //     }
    // };

    // vector<Vertex> vertices={
    //     Vertex{vec2{1,1},vec3{1,0,0}},
    //     Vertex{vec2{-1,1},vec3{0,1,0}},
    //     Vertex{vec2{0,-1},vec3{0,0,1}}
    // };

    // auto vertex_buffer=vk::Buffer::from_iter(
    //     vk::BufferAllocateInfo{
    //         .buffer_usage=vk::BufferUsage::VertexBuffer,
    //         .memory_usage=vk::MemoryUsage::Upload
    //     },
    //     vertices
    // )
    // .unwrap();

    struct P{
        vec2 pos;
        static auto per_vertex()->vk::VertexBufferDescription{
            vector member{
                vk::VertexMemberInfo{
                    .offset=0,
                    .format=vk::Format::R32G32_SFLOAT
                }
            };
            return vk::VertexBufferDescription{
                .member=member,
                .stride=sizeof(P),
                .input_rate=vk::VertexInputRate::Vertex
            };
        }
    };
    struct C{
        vec3 col;
        static auto per_vertex()->vk::VertexBufferDescription{
            vector member{
                vk::VertexMemberInfo{
                    .offset=0,
                    .format=vk::Format::R32G32B32_SFLOAT
                }
            };
            return vk::VertexBufferDescription{
                .member=member,
                .stride=sizeof(C),
                .input_rate=vk::VertexInputRate::Vertex
            };
        }
    };

    vector<P> v1={
        {vec2{1,1}},{vec2{-1,1}},{vec2{0,-1}}
    };
    vector<C> v2={
        {vec3{1,0,0}},{vec3{0,1,0}},{vec3{0,0,1}}
    };

    auto v1b=vk::Buffer::from_iter(
        vk::BufferAllocateInfo{
            .buffer_usage=vk::BufferUsage::VertexBuffer,
            .memory_usage=vk::MemoryUsage::Upload
        },
        v1
    )
    .unwrap();

    auto v2b=vk::Buffer::from_iter(
        vk::BufferAllocateInfo{
            .buffer_usage=vk::BufferUsage::VertexBuffer,
            .memory_usage=vk::MemoryUsage::Upload
        },
        v2
    )
    .unwrap();


    auto pipeline=vk::GraphicsPipeline::new_()
        .render_pass(vk::RenderingCreateInfo{
            .color_attachment_formats=vector{vk::swapchain_image_format()}
        })
        // .vertex_input_state(vector{Vertex::per_vertex()})
        .vertex_input_state(vector{P::per_vertex(),C::per_vertex()})
        .input_assembly_state((vk::InputAssemblyState::new_()))
        .vertex_shader(vk::ShaderModule::from_file("shaders/hello_triangle_vert.spv").unwrap())
        .viewport_state(vk::ViewportState::viewport_fixed_scissor_irrelevant(vector{vk::Viewport::dimension(vk::window_width(),vk::window_height())}))
        .fragment_shader(vk::ShaderModule::from_file("shaders/hello_triangle_frag.spv").unwrap())
        .build().unwrap();

    vector<vk::CommandBuffer> cmds(vk::num_swapchain_image());
    u32 swapchain_idx=0;
    for(auto& cmd: cmds){
        cmd.new_().unwrap()
            .pipeline_barrier(vk::Dependency{
                .image_barriers=vector{vk::ImageBarrier{
                    .image=vk::swapchain_image(swapchain_idx),
                    .dst_stage=vk::PipelineStage::COLOR_ATTACHMENT_OUTPUT,
                    .dst_access=vk::AccessFlag::COLOR_ATTACHMENT_WRITE,
                    .new_layout=vk::ImageLayout::AttachmentOptimal,
                }}
            })
            .begin_rendering(vk::RenderingInfo{
                .color_attachments=vector{vk::RenderingAttachmentInfo{
                    .image_view=vk::swapchain_image(swapchain_idx).image_view,
                    .load_op=vk::LoadOp::Clear,
                }}
            })
            .bind_graphics_pipeline(pipeline.handle)
            // .bind_vertex_buffer(vector{vertex_buffer})
            // .draw(vertices.size(),1,0,0)
            .bind_vertex_buffer(vector{v1b,v2b})
            .draw(v1.size(),1,0,0)
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

        swapchain_idx++;
    }

    struct FrameData{
        vk::Semaphore acq_img=vk::create_semaphore().unwrap();
        vk::Semaphore cmd_exec=vk::create_semaphore().unwrap();
        vk::Fence cpu_wait_cmd=vk::create_fence(true).unwrap();
    }frame_data[3];
    u32 frame_idx=0;

    while(!vk::window_should_close()){
        vk::poll_events();

        vk::acquire_next_image(Some(frame_data[frame_idx].acq_img),None(),swapchain_idx);
        vk::wait_for_fence(frame_data[frame_idx].cpu_wait_cmd);
        vk::reset_fence(frame_data[frame_idx].cpu_wait_cmd);

        vk::queue_submit(vk::SubmitInfo{
            .waiting=vector{frame_data[frame_idx].acq_img},
            .command_buffers=vector{cmds[swapchain_idx]},
            .signal=vector{frame_data[frame_idx].cmd_exec}
        },
        frame_data[frame_idx].cpu_wait_cmd);

        vk::queue_present(vk::PresentInfo{
            .waiting=vector{frame_data[frame_idx].cmd_exec},
            .swapchain_image_idx=swapchain_idx});

        frame_idx=(frame_idx+1)%3;
    }
}

void Engine::cleanup(){
    vk::cleanup();
}