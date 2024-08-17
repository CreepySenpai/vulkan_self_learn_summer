#pragma once

#include <span>
#include <vulkan/vulkan.hpp>


namespace Creepy{
    class Shader{
        public:
            Shader() = default;
            Shader(const vk::Device device, std::span<const char> shaderData, vk::ShaderStageFlagBits shaderStage);

            void Destroy(const vk::Device device) const;

            vk::ShaderModule GetShaderModule() const;
        private:
            vk::ShaderEXT m_shader;
            vk::ShaderModule m_shaderModule;
    };
}