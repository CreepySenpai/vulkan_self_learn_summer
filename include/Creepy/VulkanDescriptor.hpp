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

    class DescriptorSetWriter{
        public:
            template <BufferType bufferType, typename T, vk::BufferUsageFlagBits... bufferUsages>
            void AddBufferBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, vk::DescriptorType descriptorType, const BufferWrapperNoView<bufferType, T, bufferUsages...>& buffer){
                const vk::DescriptorBufferInfo bufferInfo = buffer.GetDescriptorBuffer();
                vk::WriteDescriptorSet writer{};
                writer.descriptorCount = 1;
                writer.descriptorType = descriptorType;
                writer.dstBinding = binding;
                writer.dstSet = descriptorSet;
                writer.pBufferInfo = &bufferInfo;

                m_writers.push_back(std::move(writer));
            }

            void AddImageBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, vk::DescriptorType descriptorType, const class Texture& texture);

            void UpdateDescriptorSets(const vk::Device device);
        private:
            std::vector<vk::WriteDescriptorSet> m_writers;
    };
}