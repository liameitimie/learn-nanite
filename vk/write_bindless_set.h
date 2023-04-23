#pragma once
#include "buffer/buffer.h"
#include "image/image_view.h"
#include "sampler.h"
#include "descriptor_set.h"

namespace vk{

struct CombinedImageSampler{
    Sampler* sampler;
    ImageView* image_view;
};

void write_bindless_set(u32 idx,Buffer buffer);
void write_bindless_set(u32 idx,Buffer* buffer,u32 buffer_count);
// void write_bindless_set(u32 idx,ImageView imgv);
// void write_bindless_set(u32 idx,ImageView* imgv,u32 imgv_count);

void write_bindless_set(u32 idx,CombinedImageSampler img_sampler);
void write_bindless_set(u32 idx,CombinedImageSampler* img_sampler,u32 img_sampler_count);

}