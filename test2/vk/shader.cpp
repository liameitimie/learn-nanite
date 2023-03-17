#include "shader.h"
#include "vk_context.h"
#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk{

using namespace std;

Result<ShaderModule,Error> ShaderModule::from_file(const char* path){
    ifstream file(path,ios::ate|ios::binary);
    if(!file.is_open()){
        return Err(Error::Unknown);
    }
    size_t code_size=(size_t)file.tellg();
    vector<char> code(code_size);
    file.seekg(0);
    file.read(code.data(),code_size);
    file.close();

    VkShaderModule shader;
    VkShaderModuleCreateInfo shader_desc={
        .sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize=code_size,
        .pCode=(uint32_t*)code.data(),
    };
    auto res=vkCreateShaderModule((VkDevice)device(),&shader_desc,nullptr,&shader);
    if(res!=VK_SUCCESS) return Err((Error)res);
    return Ok(ShaderModule{(u64)shader});
}

}