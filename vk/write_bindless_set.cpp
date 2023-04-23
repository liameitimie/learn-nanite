#include "write_bindless_set.h"
#include "vk_context.h"
#include <vulkan/vulkan.h>

namespace vk{

void write_bindless_set(u32 idx,Buffer buffer){
    VkDescriptorBufferInfo buffer_info={
        .buffer=(VkBuffer)buffer.handle,
        .offset=0,
        .range=buffer.size
    };
    VkWriteDescriptorSet write_desc={
        .sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet=(VkDescriptorSet)bindless_buffer_set(),
        .dstBinding=0,
        .dstArrayElement=idx,
        .descriptorCount=1,
        .descriptorType=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pBufferInfo=&buffer_info
    };
    vkUpdateDescriptorSets((VkDevice)device(),1,&write_desc,0,nullptr);
}


void write_bindless_set(u32 idx,Buffer* buffers,u32 buffer_count){
    vector<VkDescriptorBufferInfo> buffer_infos(buffer_count);
    u32 i=0;
    for(auto& buffer_info:buffer_infos){
        buffer_info=VkDescriptorBufferInfo{
            .buffer=(VkBuffer)buffers[i].handle,
            .offset=0,
            .range=buffers[i].size
        };
        i++;
    }
    VkWriteDescriptorSet write_desc={
        .sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet=(VkDescriptorSet)bindless_buffer_set(),
        .dstBinding=0,
        .dstArrayElement=idx,
        .descriptorCount=buffer_count,
        .descriptorType=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pBufferInfo=buffer_infos.data()
    };
    vkUpdateDescriptorSets((VkDevice)device(),1,&write_desc,0,nullptr);
}

void write_bindless_set(u32 idx,CombinedImageSampler img_sampler){
    VkDescriptorImageInfo image_info={
        .sampler=(VkSampler)img_sampler.sampler->handle,
        .imageView=(VkImageView)img_sampler.image_view->handle,
        .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    VkWriteDescriptorSet write_desc={
        .sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet=(VkDescriptorSet)bindless_image_set(),
        .dstBinding=0,
        .dstArrayElement=idx,
        .descriptorCount=1,
        .descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo=&image_info
    };
    vkUpdateDescriptorSets((VkDevice)device(),1,&write_desc,0,nullptr);
}

void write_bindless_set(u32 idx,CombinedImageSampler* img_sampler,u32 img_sampler_count){
    vector<VkDescriptorImageInfo> image_infos(img_sampler_count);
    u32 i=0;
    for(auto& image_info:image_infos){
        image_info=VkDescriptorImageInfo{
            .sampler=(VkSampler)img_sampler[i].sampler->handle,
            .imageView=(VkImageView)img_sampler[i].image_view->handle,
            .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };
        i++;
    }
    VkWriteDescriptorSet write_desc={
        .sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet=(VkDescriptorSet)bindless_image_set(),
        .dstBinding=0,
        .dstArrayElement=idx,
        .descriptorCount=img_sampler_count,
        .descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo=image_infos.data()
    };
    vkUpdateDescriptorSets((VkDevice)device(),1,&write_desc,0,nullptr);
}

// void write_bindless_set(u32 idx,ImageView imgv){
//     VkDescriptorImageInfo image_info={
//         .sampler=(VkSampler)default_sampler(),
//         .imageView=(VkImageView)imgv.handle,
//         .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
//     };
//     VkWriteDescriptorSet write_desc={
//         .sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
//         .dstSet=(VkDescriptorSet)bindless_image_set(),
//         .dstBinding=0,
//         .dstArrayElement=idx,
//         .descriptorCount=1,
//         .descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//         .pImageInfo=&image_info
//     };
//     vkUpdateDescriptorSets((VkDevice)device(),1,&write_desc,0,nullptr);
// }

// void write_bindless_set(u32 idx,ImageView* imgv,u32 imgv_count){
//     vector<VkDescriptorImageInfo> image_infos(imgv_count);
//     u32 i=0;
//     for(auto& image_info:image_infos){
//         image_info=VkDescriptorImageInfo{
//             .sampler=(VkSampler)default_sampler(),
//             .imageView=(VkImageView)imgv[i].handle,
//             .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
//         };
//         i++;
//     }
//     VkWriteDescriptorSet write_desc={
//         .sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
//         .dstSet=(VkDescriptorSet)bindless_image_set(),
//         .dstBinding=0,
//         .dstArrayElement=idx,
//         .descriptorCount=imgv_count,
//         .descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//         .pImageInfo=image_infos.data()
//     };
//     vkUpdateDescriptorSets((VkDevice)device(),1,&write_desc,0,nullptr);
// }

}