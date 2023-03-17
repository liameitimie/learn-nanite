#pragma once
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

inline void enum_instance_extension(){
    std::vector<VkExtensionProperties> exts;
    uint32_t ext_cnt=0;
    vkEnumerateInstanceExtensionProperties(nullptr,&ext_cnt,nullptr);
    exts.resize(ext_cnt);
    vkEnumerateInstanceExtensionProperties(nullptr,&ext_cnt,exts.data());

    std::cout<<"enum instance extensions:\n";
    for(auto& ext:exts){
        std::cout<<"\t"<<ext.extensionName<<' '<<ext.specVersion<<"\n";
    }
    std::cout<<std::endl;
}

inline void enum_instance_layer(){
    uint32_t layer_cnt=0;
    vkEnumerateInstanceLayerProperties(&layer_cnt,nullptr);
    std::vector<VkLayerProperties> layers(layer_cnt);
    vkEnumerateInstanceLayerProperties(&layer_cnt,layers.data());

    std::cout<<"enum insance layers:\n";
    for(auto& layer:layers){
        std::cout<<"\t"<<layer.layerName<<", "<<layer.description<<", "<<layer.specVersion<<' '<<layer.implementationVersion<<'\n';
    }
    std::cout<<std::endl;
}

inline void enum_device_extension(VkPhysicalDevice adapter){
    uint32_t ext_cnt=0;
    vkEnumerateDeviceExtensionProperties(adapter,nullptr,&ext_cnt,nullptr);
    std::vector<VkExtensionProperties> exts(ext_cnt);
    vkEnumerateDeviceExtensionProperties(adapter,nullptr,&ext_cnt,exts.data());

    std::cout<<"enum device extensions:\n";
    for(auto& ext:exts){
        std::cout<<"\t"<<ext.extensionName<<' '<<ext.specVersion<<"\n";
    }
    std::cout<<std::endl;
}

inline void enum_physical_device(VkInstance instance){
    std::vector<VkPhysicalDevice> phy_devs;
    uint32_t dev_cnt=0;
    vkEnumeratePhysicalDevices(instance,&dev_cnt,nullptr);
    phy_devs.resize(dev_cnt);
    vkEnumeratePhysicalDevices(instance,&dev_cnt,phy_devs.data());

    std::cout<<"enum_physical_device:\n";
    for(auto& dev:phy_devs){
        VkPhysicalDeviceProperties dev_prop;
        VkPhysicalDeviceFeatures dev_features;
        vkGetPhysicalDeviceProperties(dev,&dev_prop);
        vkGetPhysicalDeviceFeatures(dev,&dev_features);

        std::cout<<"\tdevice name: "<<dev_prop.deviceName<<"\n";
        std::cout<<"\tapi version: "<<dev_prop.apiVersion<<"\n";
        std::cout<<"\tvendor ID: "<<dev_prop.vendorID<<"\n";
        std::cout<<"\tdevice ID: "<<dev_prop.deviceID<<"\n";
        std::cout<<"\tdevice type: ";
        switch(dev_prop.deviceType){
            case VK_PHYSICAL_DEVICE_TYPE_OTHER: std::cout<<"other\n"; break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: std::cout<<"integrated gpu\n"; break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: std::cout<<"discrete gpu\n"; break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: std::cout<<"virtual gpu\n"; break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU: std::cout<<"cpu\n"; break;
            case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
                break;
        }
        // todo: dev_prop没输出完，后面的很长

        std::string device_feature_names[]={
            "robustBufferAccess",
            "fullDrawIndexUint32",
            "imageCubeArray",
            "independentBlend",
            "geometryShader",
            "tessellationShader",
            "sampleRateShading",
            "dualSrcBlend",
            "logicOp",
            "multiDrawIndirect",
            "drawIndirectFirstInstance",
            "depthClamp",
            "depthBiasClamp",
            "fillModeNonSolid",
            "depthBounds",
            "wideLines",
            "largePoints",
            "alphaToOne",
            "multiViewport",
            "samplerAnisotropy",
            "textureCompressionETC2",
            "textureCompressionASTC_LDR",
            "textureCompressionBC",
            "occlusionQueryPrecise",
            "pipelineStatisticsQuery",
            "vertexPipelineStoresAndAtomics",
            "fragmentStoresAndAtomics",
            "shaderTessellationAndGeometryPointSize",
            "shaderImageGatherExtended",
            "shaderStorageImageExtendedFormats",
            "shaderStorageImageMultisample",
            "shaderStorageImageReadWithoutFormat",
            "shaderStorageImageWriteWithoutFormat",
            "shaderUniformBufferArrayDynamicIndexing",
            "shaderSampledImageArrayDynamicIndexing",
            "shaderStorageBufferArrayDynamicIndexing",
            "shaderStorageImageArrayDynamicIndexing",
            "shaderClipDistance",
            "shaderCullDistance",
            "shaderFloat64",
            "shaderInt64",
            "shaderInt16",
            "shaderResourceResidency",
            "shaderResourceMinLod",
            "sparseBinding",
            "sparseResidencyBuffer",
            "sparseResidencyImage2D",
            "sparseResidencyImage3D",
            "sparseResidency2Samples",
            "sparseResidency4Samples",
            "sparseResidency8Samples",
            "sparseResidency16Samples",
            "sparseResidencyAliased",
            "variableMultisampleRate",
            "inheritedQueries",
        };

        std::cout<<"\tdevice features support: {\n\t\t";
        VkBool32 *p=(VkBool32*)&dev_features;
        for(int i=0;i<sizeof(dev_features)/sizeof(VkBool32);i++){
            if(p[i]) std::cout<<'1';
            else std::cout<<'0';
        }
        std::cout<<"\n\t\tunsupport features: ";
        for(int i=0;i<sizeof(dev_features)/sizeof(VkBool32);i++){
            if(!p[i]) std::cout<<device_feature_names[i]<<' ';
        }
        std::cout<<"\n";
        std::cout<<"\t}\n";

        std::vector<VkQueueFamilyProperties> qfam_props;
        uint32_t qfam_cnt=0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev,&qfam_cnt,nullptr);
        qfam_props.resize(qfam_cnt);
        vkGetPhysicalDeviceQueueFamilyProperties(dev,&qfam_cnt,qfam_props.data());
        
        std::cout<<"\tqueue family properties: {\n";
        for(auto& qfam_prop:qfam_props){
            std::cout<<"\t\tqueue count: "<<qfam_prop.queueCount<<"\n";
            std::cout<<"\t\tqueue: ";
            if(qfam_prop.queueFlags&VK_QUEUE_GRAPHICS_BIT) std::cout<<"graphics, ";
            if(qfam_prop.queueFlags&VK_QUEUE_COMPUTE_BIT) std::cout<<"compute, ";
            if(qfam_prop.queueFlags&VK_QUEUE_TRANSFER_BIT) std::cout<<"transfer, ";
            if(qfam_prop.queueFlags&VK_QUEUE_SPARSE_BINDING_BIT) std::cout<<"sparse binding, ";
            if(qfam_prop.queueFlags&VK_QUEUE_PROTECTED_BIT) std::cout<<"protected, ";
            if(qfam_prop.queueFlags&VK_QUEUE_OPTICAL_FLOW_BIT_NV) std::cout<<"optical flow nv, ";
            std::cout<<"\n";
        }
        std::cout<<"\t}\n";

        std::cout<<"\n";
    }
    std::cout<<std::endl;
}

inline void query_surface_capabilities(VkPhysicalDevice adp,VkSurfaceKHR surface){
    VkSurfaceCapabilitiesKHR surface_detail;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adp,surface,&surface_detail);
    
    uint32_t format_cnt;
    vkGetPhysicalDeviceSurfaceFormatsKHR(adp,surface,&format_cnt,nullptr);
    std::vector<VkSurfaceFormatKHR> formats(format_cnt);
    vkGetPhysicalDeviceSurfaceFormatsKHR(adp,surface,&format_cnt,formats.data());

    uint32_t pre_mode_cnt;
    vkGetPhysicalDeviceSurfacePresentModesKHR(adp,surface,&pre_mode_cnt,nullptr);
    std::vector<VkPresentModeKHR> pre_modes(pre_mode_cnt);
    vkGetPhysicalDeviceSurfacePresentModesKHR(adp,surface,&pre_mode_cnt,pre_modes.data());

}

/*
    VkSurfaceCapabilitiesKHR sfc;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adp,surface,&sfc);
    
    uint32_t format_cnt=0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(adp,surface,&format_cnt,nullptr);
    std::vector<VkSurfaceFormatKHR> formats(format_cnt);
    vkGetPhysicalDeviceSurfaceFormatsKHR(adp,surface,&format_cnt,formats.data());

    uint32_t pre_mode_cnt=0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(adp,surface,&pre_mode_cnt,nullptr);
    std::vector<VkPresentModeKHR> pre_modes(format_cnt);
    vkGetPhysicalDeviceSurfacePresentModesKHR(adp,surface,&pre_mode_cnt,pre_modes.data());


*/