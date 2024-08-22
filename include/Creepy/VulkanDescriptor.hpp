#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include "VulkanBuffer.hpp"

namespace Creepy {
    
    struct DescriptorSet{
        vk::DescriptorSet DescriptorSet;
        vk::DescriptorSetLayout DescriptorSetLayout;
    };

    class DescriptorSetBuilder{
        public:
            void AddBinding(const uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags shaderStage);
            
            vk::DescriptorSetLayout BuildDescriptorLayout(const vk::Device device);

            DescriptorSet AllocateDescriptorSet(const vk::Device device, const vk::DescriptorPool descriptorPool);
        private:
            std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
            vk::DescriptorSetLayout m_descriptorSetLayout{};
    };

    // class DescriptorImage{
    //     public:

    //     private:
    // };

    // class DescriptorSetWriter{
    //     public:
    //         void AddBufferBinding();

    //         void AddImageBinding();

    //         void UpdateDescriptorSets(const vk::Device device);
    //     private:
    //         std::vector<vk::WriteDescriptorSet> m_writers;
    // };
}