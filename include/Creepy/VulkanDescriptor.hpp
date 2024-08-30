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
    concept IsBuffer = std::same_as<UniformBuffer::TransformBuffer, std::remove_cvref_t<T>> || 
                        std::same_as<UniformBuffer::LightBuffer, std::remove_cvref_t<T>>;

    template <typename T>
    concept IsTexture = std::same_as<Texture, std::remove_cvref_t<T>>;

    struct DescriptorBufferInfo{
        constexpr DescriptorBufferInfo() = default;

        constexpr DescriptorBufferInfo(const uint32_t binding, const uint32_t descriptorCount, const vk::DescriptorType descriptorType, const IsBuffer auto& buffer)
            : m_binding{binding}, m_descriptorCount{descriptorCount}, m_descriptorType{descriptorType}, m_bufferInfo{buffer.GetBuffer(), buffer.GetBufferOffset(), buffer.GetBufferSize()}
        {
            
        }

        uint32_t m_binding{};
        uint32_t m_descriptorCount{};
        vk::DescriptorType m_descriptorType{};
        vk::DescriptorBufferInfo m_bufferInfo{};
    };

    struct DescriptorImageInfo{
        constexpr DescriptorImageInfo() = default;

        constexpr DescriptorImageInfo(const uint32_t binding, const uint32_t descriptorCount, const vk::DescriptorType descriptorType, const IsTexture auto& texture)
            : m_binding{binding}, m_descriptorCount{descriptorCount}, m_descriptorType{descriptorType}, m_imageInfo{texture.GetSampler(), texture.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal}
        {

        }

        uint32_t m_binding{};
        uint32_t m_descriptorCount{};
        vk::DescriptorType m_descriptorType{};
        vk::DescriptorImageInfo m_imageInfo{};
    };

    class DescriptorBufferInfoBuilder{
        public:
            constexpr void AddBinding(const uint32_t binding, const uint32_t descriptorCount, const vk::DescriptorType descriptorType, const IsBuffer auto& buffer){
                m_descriptorBufferInfos.emplace_back(binding, descriptorCount, descriptorType, buffer);
            }

            friend class DescriptorSetWriter;
        private:
            std::vector<DescriptorBufferInfo> m_descriptorBufferInfos;
    };

    class DescriptorImageInfoBuilder{
        public:
            constexpr void AddBinding(const uint32_t binding, const uint32_t descriptorCount, const vk::DescriptorType descriptorType, const IsTexture auto& texture) {
                m_descriptorImageInfos.emplace_back(binding, descriptorCount, descriptorType, texture);
            }

            friend class DescriptorSetWriter;
        private:
            std::vector<DescriptorImageInfo> m_descriptorImageInfos;
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