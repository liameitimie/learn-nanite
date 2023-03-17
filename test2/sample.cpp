#include "sample.h"
#include <vector>

using namespace std;

vk::GraphicsPipeline create_pipeline(Engine &engine){
    auto pipeline=vk::GraphicsPipeline::new_()
        .render_pass(vk::RenderingCreateInfo{
            .color_attachment_formats=vector{vk::swapchain_image_format()}
        })
        .input_assembly_state((vk::InputAssemblyState::new_()))
        .vertex_shader(vk::ShaderModule::from_file("shaders/hello_triangle_vert.spv").unwrap())
        .viewport_state(vk::ViewportState::viewport_fixed_scissor_irrelevant(vector{vk::Viewport::dimension(engine.window_extent.x,engine.window_extent.y)}))
        .fragment_shader(vk::ShaderModule::from_file("shaders/hello_triangle_frag.spv").unwrap())
        .build().unwrap();
    return pipeline;
}

vk::CommandBuffer create_cmd(Engine &engine,u32 swapchain_idx,vk::GraphicsPipeline pipeline){
    vk::CommandBuffer cmd;
    cmd.new_().unwrap();
        cmd.pipeline_barrier(vk::Dependency{
            .image_barriers=vector{vk::ImageBarrier{
                .image=vk::swapchain_image(swapchain_idx),
                .dst_access=vk::AccessFlag::COLOR_ATTACHMENT_WRITE,
                .new_layout=vk::ImageLayout::AttachmentOptimal,
            }}
        });
        cmd.begin_rendering(vk::RenderingInfo{
            .color_attachments=vector<vk::RenderingAttachmentInfo>{
                 vk::RenderingAttachmentInfo{
            //     .image_view=vk::swapchain_image(swapchain_idx).image_view,
            //     .load_op=vk::LoadOp::Clear,
            //     .clear_value=Some(vk::ClearValue(vk::ClearColorValue(vec4{0.0f,0.0f,0.0f,1.0f})))
             }
            }
        });
        cmd.bind_graphics_pipeline(pipeline.handle);
        cmd.draw(3,1,0,0);
        cmd.end_rendering();
        cmd.pipeline_barrier(vk::Dependency{
            .image_barriers=vector{vk::ImageBarrier{
                .image=vk::swapchain_image(swapchain_idx),
                .src_access=vk::AccessFlag::COLOR_ATTACHMENT_WRITE,
                .old_layout=vk::ImageLayout::AttachmentOptimal,
                .new_layout=vk::ImageLayout::PresentSrc
            }}
        });
        cmd.build().unwrap();

    return cmd;
}