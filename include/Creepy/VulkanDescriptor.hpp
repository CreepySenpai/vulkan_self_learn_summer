#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Creepy {
    struct DescriptorSet{
        vk::DescriptorSet m_descriptorSet;
        vk::DescriptorSetLayout m_descriptorSetLayout;
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

    class DescriptorWriter{
        public:
            void AddBufferBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, vk::DescriptorType descriptorType);
            void AddImageBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, vk::DescriptorType descriptorType);

            void UpdateDescriptorSets(const vk::Device device);
        private:
            std::vector<vk::WriteDescriptorSet> m_writers;
    };
}