#pragma once

#include <span>
#include <vulkan/vulkan.hpp>


class Shader{
    public:
        Shader() = default;
        Shader(const vk::Device device, std::span<const char> shaderData, vk::ShaderStageFlagBits shaderStage);

        void Destroy(const vk::Device device);
    private:
        vk::ShaderEXT m_shader;
        vk::ShaderModule m_shaderModule;
};