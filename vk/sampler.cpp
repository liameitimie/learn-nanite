#include "sampler.h"
#include <vulkan/vulkan.h>
#include "vk_context.h"

namespace vk{

Result<Sampler,Error> create_sampler(SamplerDesc sampler_desc){
    Sampler sampler{
        .unnorm_coord=sampler_desc.unnorm_coord
    };
    VkSamplerCreateInfo vk_sampler_desc{
        .sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .addressModeU=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        .addressModeV=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        .addressModeW=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        .maxLod=sampler_desc.unnorm_coord?0:VK_LOD_CLAMP_NONE,
        .unnormalizedCoordinates=sampler_desc.unnorm_coord
    };
    auto res=vkCreateSampler((VkDevice)device(),&vk_sampler_desc,nullptr,(VkSampler*)&sampler.handle);
    if(res) return Err(Error(res));
    return Ok(sampler);
}

}