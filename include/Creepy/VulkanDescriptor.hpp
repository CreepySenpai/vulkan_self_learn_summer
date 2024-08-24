#pragma once

#include <vector>
#include <concepts>
#include <vulkan/vulkan.hpp>
#include "VulkanBuffer.hpp"
#include "Texture.hpp"

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

    template <typename T>
    concept IsBuffer = std::same_as<UniformBuffer, std::remove_cvref_t<T>>;

    class DescriptorBufferInfoBuilder{
        public:
            DescriptorBufferInfoBuilder(IsBuffer auto&&... buffers){
                m_descriptorBufferInfo.reserve(sizeof...(buffers));

                (m_descriptorBufferInfo.emplace_back(buffers.GetBuffer(), buffers.GetBufferOffset(), buffers.GetBufferSize()), ...);
            }

            friend class DescriptorSetWriter;
        private:
            std::vector<vk::DescriptorBufferInfo> m_descriptorBufferInfo;
    };

    template <typename T>
    concept IsTexture = std::same_as<Texture, std::remove_cvref_t<T>>;

    class DescriptorImageInfoBuilder{
        public:
            DescriptorImageInfoBuilder(IsTexture auto&&... textures){
                m_descriptorImageInfos.reserve(sizeof...(textures));

                (m_descriptorImageInfos.emplace_back(textures.GetSampler(), textures.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal), ...);
            }

            friend class DescriptorSetWriter;
        private:
            std::vector<vk::DescriptorImageInfo> m_descriptorImageInfos;
    };

    class DescriptorSetWriter{
        public:
            void AddBufferBinding(const vk::DescriptorSet descriptorSet, const DescriptorBufferInfoBuilder& bufferInfos);

            void AddImageBinding(const vk::DescriptorSet descriptorSet, const DescriptorImageInfoBuilder& imageInfos);

            void UpdateDescriptorSets(const vk::Device device);
        private:
            std::vector<vk::WriteDescriptorSet> m_writers;
    };
}