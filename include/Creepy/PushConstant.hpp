#pragma once

#include <vulkan/vulkan.hpp>

namespace Creepy{

    struct FragmentPushConstantData{
        vk::DeviceAddress lightBufferPtr;
        vk::DeviceAddress materialBufferPtr;
        uint32_t diffuseTextureIndex;
    };

}