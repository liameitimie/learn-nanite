#pragma once
#include "../format.h"
#include <types.h>

namespace vk{

enum ImageAspect{
    Color = 0x00000001,
    Depth = 0x00000002,
    Stencil = 0x00000004,
    Metadata = 0x00000008,
    Plane0 = 0x00000010,
    Plane1 = 0x00000020,
    Plane2 = 0x00000040,
    None = 0,
    MemoryPlane0 = 0x00000080,
    MemoryPlane1 = 0x00000100,
    MemoryPlane2 = 0x00000200,
    MemoryPlane3 = 0x00000400,
};

inline u32 aspect(Format format){
    switch(format){
    case Format::UNDEFINED: return ImageAspect::None;

    case Format::R4G4_UNORM_PACK8: return ImageAspect::Color;
    case Format::R4G4B4A4_UNORM_PACK16: return ImageAspect::Color;
    case Format::B4G4R4A4_UNORM_PACK16: return ImageAspect::Color;
    case Format::R5G6B5_UNORM_PACK16: return ImageAspect::Color;
    case Format::B5G6R5_UNORM_PACK16: return ImageAspect::Color;
    case Format::R5G5B5A1_UNORM_PACK16: return ImageAspect::Color;
    case Format::B5G5R5A1_UNORM_PACK16: return ImageAspect::Color;
    case Format::A1R5G5B5_UNORM_PACK16: return ImageAspect::Color;
    case Format::R8_UNORM: return ImageAspect::Color;
    case Format::R8_SNORM: return ImageAspect::Color;
    case Format::R8_USCALED: return ImageAspect::Color;
    case Format::R8_SSCALED: return ImageAspect::Color;
    case Format::R8_UINT: return ImageAspect::Color;
    case Format::R8_SINT: return ImageAspect::Color;
    case Format::R8_SRGB: return ImageAspect::Color;
    case Format::R8G8_UNORM: return ImageAspect::Color;
    case Format::R8G8_SNORM: return ImageAspect::Color;
    case Format::R8G8_USCALED: return ImageAspect::Color;
    case Format::R8G8_SSCALED: return ImageAspect::Color;
    case Format::R8G8_UINT: return ImageAspect::Color;
    case Format::R8G8_SINT: return ImageAspect::Color;
    case Format::R8G8_SRGB: return ImageAspect::Color;
    case Format::R8G8B8_UNORM: return ImageAspect::Color;
    case Format::R8G8B8_SNORM: return ImageAspect::Color;
    case Format::R8G8B8_USCALED: return ImageAspect::Color;
    case Format::R8G8B8_SSCALED: return ImageAspect::Color;
    case Format::R8G8B8_UINT: return ImageAspect::Color;
    case Format::R8G8B8_SINT: return ImageAspect::Color;
    case Format::R8G8B8_SRGB: return ImageAspect::Color;
    case Format::B8G8R8_UNORM: return ImageAspect::Color;
    case Format::B8G8R8_SNORM: return ImageAspect::Color;
    case Format::B8G8R8_USCALED: return ImageAspect::Color;
    case Format::B8G8R8_SSCALED: return ImageAspect::Color;
    case Format::B8G8R8_UINT: return ImageAspect::Color;
    case Format::B8G8R8_SINT: return ImageAspect::Color;
    case Format::B8G8R8_SRGB: return ImageAspect::Color;
    case Format::R8G8B8A8_UNORM: return ImageAspect::Color;
    case Format::R8G8B8A8_SNORM: return ImageAspect::Color;
    case Format::R8G8B8A8_USCALED: return ImageAspect::Color;
    case Format::R8G8B8A8_SSCALED: return ImageAspect::Color;
    case Format::R8G8B8A8_UINT: return ImageAspect::Color;
    case Format::R8G8B8A8_SINT: return ImageAspect::Color;
    case Format::R8G8B8A8_SRGB: return ImageAspect::Color;
    case Format::B8G8R8A8_UNORM: return ImageAspect::Color;
    case Format::B8G8R8A8_SNORM: return ImageAspect::Color;
    case Format::B8G8R8A8_USCALED: return ImageAspect::Color;
    case Format::B8G8R8A8_SSCALED: return ImageAspect::Color;
    case Format::B8G8R8A8_UINT: return ImageAspect::Color;
    case Format::B8G8R8A8_SINT: return ImageAspect::Color;
    case Format::B8G8R8A8_SRGB: return ImageAspect::Color;
    case Format::A8B8G8R8_UNORM_PACK32: return ImageAspect::Color;
    case Format::A8B8G8R8_SNORM_PACK32: return ImageAspect::Color;
    case Format::A8B8G8R8_USCALED_PACK32: return ImageAspect::Color;
    case Format::A8B8G8R8_SSCALED_PACK32: return ImageAspect::Color;
    case Format::A8B8G8R8_UINT_PACK32: return ImageAspect::Color;
    case Format::A8B8G8R8_SINT_PACK32: return ImageAspect::Color;
    case Format::A8B8G8R8_SRGB_PACK32: return ImageAspect::Color;
    case Format::A2R10G10B10_UNORM_PACK32: return ImageAspect::Color;
    case Format::A2R10G10B10_SNORM_PACK32: return ImageAspect::Color;
    case Format::A2R10G10B10_USCALED_PACK32: return ImageAspect::Color;
    case Format::A2R10G10B10_SSCALED_PACK32: return ImageAspect::Color;
    case Format::A2R10G10B10_UINT_PACK32: return ImageAspect::Color;
    case Format::A2R10G10B10_SINT_PACK32: return ImageAspect::Color;
    case Format::A2B10G10R10_UNORM_PACK32: return ImageAspect::Color;
    case Format::A2B10G10R10_SNORM_PACK32: return ImageAspect::Color;
    case Format::A2B10G10R10_USCALED_PACK32: return ImageAspect::Color;
    case Format::A2B10G10R10_SSCALED_PACK32: return ImageAspect::Color;
    case Format::A2B10G10R10_UINT_PACK32: return ImageAspect::Color;
    case Format::A2B10G10R10_SINT_PACK32: return ImageAspect::Color;
    case Format::R16_UNORM: return ImageAspect::Color;
    case Format::R16_SNORM: return ImageAspect::Color;
    case Format::R16_USCALED: return ImageAspect::Color;
    case Format::R16_SSCALED: return ImageAspect::Color;
    case Format::R16_UINT: return ImageAspect::Color;
    case Format::R16_SINT: return ImageAspect::Color;
    case Format::R16_SFLOAT: return ImageAspect::Color;
    case Format::R16G16_UNORM: return ImageAspect::Color;
    case Format::R16G16_SNORM: return ImageAspect::Color;
    case Format::R16G16_USCALED: return ImageAspect::Color;
    case Format::R16G16_SSCALED: return ImageAspect::Color;
    case Format::R16G16_UINT: return ImageAspect::Color;
    case Format::R16G16_SINT: return ImageAspect::Color;
    case Format::R16G16_SFLOAT: return ImageAspect::Color;
    case Format::R16G16B16_UNORM: return ImageAspect::Color;
    case Format::R16G16B16_SNORM: return ImageAspect::Color;
    case Format::R16G16B16_USCALED: return ImageAspect::Color;
    case Format::R16G16B16_SSCALED: return ImageAspect::Color;
    case Format::R16G16B16_UINT: return ImageAspect::Color;
    case Format::R16G16B16_SINT: return ImageAspect::Color;
    case Format::R16G16B16_SFLOAT: return ImageAspect::Color;
    case Format::R16G16B16A16_UNORM: return ImageAspect::Color;
    case Format::R16G16B16A16_SNORM: return ImageAspect::Color;
    case Format::R16G16B16A16_USCALED: return ImageAspect::Color;
    case Format::R16G16B16A16_SSCALED: return ImageAspect::Color;
    case Format::R16G16B16A16_UINT: return ImageAspect::Color;
    case Format::R16G16B16A16_SINT: return ImageAspect::Color;
    case Format::R16G16B16A16_SFLOAT: return ImageAspect::Color;
    case Format::R32_UINT: return ImageAspect::Color;
    case Format::R32_SINT: return ImageAspect::Color;
    case Format::R32_SFLOAT: return ImageAspect::Color;
    case Format::R32G32_UINT: return ImageAspect::Color;
    case Format::R32G32_SINT: return ImageAspect::Color;
    case Format::R32G32_SFLOAT: return ImageAspect::Color;
    case Format::R32G32B32_UINT: return ImageAspect::Color;
    case Format::R32G32B32_SINT: return ImageAspect::Color;
    case Format::R32G32B32_SFLOAT: return ImageAspect::Color;
    case Format::R32G32B32A32_UINT: return ImageAspect::Color;
    case Format::R32G32B32A32_SINT: return ImageAspect::Color;
    case Format::R32G32B32A32_SFLOAT: return ImageAspect::Color;
    case Format::R64_UINT: return ImageAspect::Color;
    case Format::R64_SINT: return ImageAspect::Color;
    case Format::R64_SFLOAT: return ImageAspect::Color;
    case Format::R64G64_UINT: return ImageAspect::Color;
    case Format::R64G64_SINT: return ImageAspect::Color;
    case Format::R64G64_SFLOAT: return ImageAspect::Color;
    case Format::R64G64B64_UINT: return ImageAspect::Color;
    case Format::R64G64B64_SINT: return ImageAspect::Color;
    case Format::R64G64B64_SFLOAT: return ImageAspect::Color;
    case Format::R64G64B64A64_UINT: return ImageAspect::Color;
    case Format::R64G64B64A64_SINT: return ImageAspect::Color;
    case Format::R64G64B64A64_SFLOAT: return ImageAspect::Color;
    case Format::B10G11R11_UFLOAT_PACK32: return ImageAspect::Color;
    case Format::E5B9G9R9_UFLOAT_PACK32: return ImageAspect::Color;
    case Format::D16_UNORM: return ImageAspect::Depth;
    case Format::X8_D24_UNORM_PACK32: return ImageAspect::Depth;
    case Format::D32_SFLOAT: return ImageAspect::Depth;
    case Format::S8_UINT: return ImageAspect::Stencil;

    case Format::D16_UNORM_S8_UINT: return ImageAspect::Depth|ImageAspect::Stencil;
    case Format::D24_UNORM_S8_UINT: return ImageAspect::Depth|ImageAspect::Stencil;
    case Format::D32_SFLOAT_S8_UINT: return ImageAspect::Depth|ImageAspect::Stencil;

    case Format::BC1_RGB_UNORM_BLOCK: return ImageAspect::Color;
    case Format::BC1_RGB_SRGB_BLOCK: return ImageAspect::Color;
    case Format::BC1_RGBA_UNORM_BLOCK: return ImageAspect::Color;
    case Format::BC1_RGBA_SRGB_BLOCK: return ImageAspect::Color;
    case Format::BC2_UNORM_BLOCK: return ImageAspect::Color;
    case Format::BC2_SRGB_BLOCK: return ImageAspect::Color;
    case Format::BC3_UNORM_BLOCK: return ImageAspect::Color;
    case Format::BC3_SRGB_BLOCK: return ImageAspect::Color;
    case Format::BC4_UNORM_BLOCK: return ImageAspect::Color;
    case Format::BC4_SNORM_BLOCK: return ImageAspect::Color;
    case Format::BC5_UNORM_BLOCK: return ImageAspect::Color;
    case Format::BC5_SNORM_BLOCK: return ImageAspect::Color;
    case Format::BC6H_UFLOAT_BLOCK: return ImageAspect::Color;
    case Format::BC6H_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::BC7_UNORM_BLOCK: return ImageAspect::Color;
    case Format::BC7_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ETC2_R8G8B8_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ETC2_R8G8B8_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ETC2_R8G8B8A1_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ETC2_R8G8B8A1_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ETC2_R8G8B8A8_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ETC2_R8G8B8A8_SRGB_BLOCK: return ImageAspect::Color;
    case Format::EAC_R11_UNORM_BLOCK: return ImageAspect::Color;
    case Format::EAC_R11_SNORM_BLOCK: return ImageAspect::Color;
    case Format::EAC_R11G11_UNORM_BLOCK: return ImageAspect::Color;
    case Format::EAC_R11G11_SNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_4x4_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_4x4_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_5x4_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_5x4_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_5x5_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_5x5_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_6x5_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_6x5_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_6x6_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_6x6_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_8x5_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_8x5_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_8x6_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_8x6_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_8x8_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_8x8_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x5_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x5_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x6_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x6_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x8_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x8_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x10_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x10_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_12x10_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_12x10_SRGB_BLOCK: return ImageAspect::Color;
    case Format::ASTC_12x12_UNORM_BLOCK: return ImageAspect::Color;
    case Format::ASTC_12x12_SRGB_BLOCK: return ImageAspect::Color;
    case Format::G8B8G8R8_422_UNORM: return ImageAspect::Color;
    case Format::B8G8R8G8_422_UNORM: return ImageAspect::Color;

    case Format::G8_B8_R8_3PLANE_420_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;
    case Format::G8_B8R8_2PLANE_420_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G8_B8_R8_3PLANE_422_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;
    case Format::G8_B8R8_2PLANE_422_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G8_B8_R8_3PLANE_444_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;

    case Format::R10X6_UNORM_PACK16: return ImageAspect::Color;
    case Format::R10X6G10X6_UNORM_2PACK16: return ImageAspect::Color;
    case Format::R10X6G10X6B10X6A10X6_UNORM_4PACK16: return ImageAspect::Color;
    case Format::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: return ImageAspect::Color;
    case Format::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: return ImageAspect::Color;

    case Format::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;
    case Format::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;
    case Format::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;

    case Format::R12X4_UNORM_PACK16: return ImageAspect::Color;
    case Format::R12X4G12X4_UNORM_2PACK16: return ImageAspect::Color;
    case Format::R12X4G12X4B12X4A12X4_UNORM_4PACK16: return ImageAspect::Color;
    case Format::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: return ImageAspect::Color;
    case Format::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: return ImageAspect::Color;

    case Format::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;
    case Format::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;
    case Format::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;

    case Format::G16B16G16R16_422_UNORM: return ImageAspect::Color;
    case Format::B16G16R16G16_422_UNORM: return ImageAspect::Color;

    case Format::G16_B16_R16_3PLANE_420_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;
    case Format::G16_B16R16_2PLANE_420_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G16_B16_R16_3PLANE_422_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;
    case Format::G16_B16R16_2PLANE_422_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G16_B16_R16_3PLANE_444_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1|ImageAspect::Plane2;

    case Format::G8_B8R8_2PLANE_444_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;
    case Format::G16_B16R16_2PLANE_444_UNORM: return ImageAspect::Color|ImageAspect::Plane0|ImageAspect::Plane1;

    case Format::A4R4G4B4_UNORM_PACK16: return ImageAspect::Color;
    case Format::A4B4G4R4_UNORM_PACK16: return ImageAspect::Color;
    case Format::ASTC_4x4_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_5x4_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_5x5_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_6x5_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_6x6_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_8x5_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_8x6_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_8x8_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x5_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x6_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x8_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_10x10_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_12x10_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::ASTC_12x12_SFLOAT_BLOCK: return ImageAspect::Color;
    case Format::PVRTC1_2BPP_UNORM_BLOCK_IMG: return ImageAspect::Color;
    case Format::PVRTC1_4BPP_UNORM_BLOCK_IMG: return ImageAspect::Color;
    case Format::PVRTC2_2BPP_UNORM_BLOCK_IMG: return ImageAspect::Color;
    case Format::PVRTC2_4BPP_UNORM_BLOCK_IMG: return ImageAspect::Color;
    case Format::PVRTC1_2BPP_SRGB_BLOCK_IMG: return ImageAspect::Color;
    case Format::PVRTC1_4BPP_SRGB_BLOCK_IMG: return ImageAspect::Color;
    case Format::PVRTC2_2BPP_SRGB_BLOCK_IMG: return ImageAspect::Color;
    case Format::PVRTC2_4BPP_SRGB_BLOCK_IMG: return ImageAspect::Color;
    case Format::R16G16_S10_5_NV: return ImageAspect::Color;
        break;
    }
}

}