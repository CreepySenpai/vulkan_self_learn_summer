#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Creepy {
    struct DescriptorSet{
        vk::DescriptorSet DescriptorSet;
        vk::DescriptorSetLayout DescriptorSetLayout;
    };

    class DescriptorSetBuilder{
        public:
            void AddBinding(const uint32_t binding, vk::DescriptorType type);
            
            void BuildDescriptorLayout(const vk::Device device, vk::ShaderStageFlags shaderStage);

            DescriptorSet AllocateDescriptorSet(const vk::Device device, const vk::DescriptorPool descriptorPool);
        private:
            std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
            vk::DescriptorSetLayout m_descriptorSetLayout{};
    };

    class DescriptorSetWriter{
        public:
            void AddBufferBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, vk::DescriptorType descriptorType, const vk::Buffer buffer, uint64_t bufferSize);
            void AddImageBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, vk::DescriptorType descriptorType, const class Texture& texture);

            void UpdateDescriptorSets(const vk::Device device);
        private:
            std::vector<vk::WriteDescriptorSet> m_writers;
    };
}