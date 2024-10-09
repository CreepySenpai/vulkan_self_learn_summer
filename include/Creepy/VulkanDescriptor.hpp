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
            void AddBinding(const uint32_t binding, const uint32_t descriptorCount, vk::DescriptorType type, vk::ShaderStageFlags shaderStage);
            
            void AddBindingWithFlag(const uint32_t binding, const uint32_t descriptorCount, vk::DescriptorType type, vk::ShaderStageFlags shaderStage, vk::DescriptorBindingFlags bindingFlags);

            vk::DescriptorSetLayout BuildDescriptorLayout(const vk::Device device);

            //TODO: Change name
            vk::DescriptorSetLayout BuildDescriptorLayoutWithFlags(const vk::Device device);

            DescriptorSet AllocateDescriptorSet(const vk::Device device, const vk::DescriptorPool descriptorPool);
            
            DescriptorSet AllocateDescriptorSetWithFlags(const vk::Device device, const vk::DescriptorPool descriptorPool);
        private:
            std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
            std::vector<vk::DescriptorBindingFlags> m_bindingFlags;
            vk::DescriptorSetLayout m_descriptorSetLayout{};
    };
    
    
    template <typename T>
    concept IsBuffer = requires(T buffer){
        {buffer.GetBuffer()} -> std::same_as<vk::Buffer>;
        {buffer.GetBufferSize()} -> std::same_as<uint64_t>;
    };

    struct DescriptorBufferInfo{
        constexpr DescriptorBufferInfo() = default;

        constexpr DescriptorBufferInfo(const uint32_t binding, const uint32_t descriptorCount, const vk::DescriptorType descriptorType, const IsBuffer auto& buffer)
            : m_binding{binding}, m_descriptorCount{descriptorCount}, m_descriptorType{descriptorType}, m_bufferInfo{buffer.GetBuffer(), 0, buffer.GetBufferSize()}
        {
            std::println("DescriptorBufferInfoSize {}", buffer.GetBufferSize());
        }

        uint32_t m_binding{};
        uint32_t m_descriptorCount{};
        vk::DescriptorType m_descriptorType{};
        vk::DescriptorBufferInfo m_bufferInfo{};
    };

    template <typename T>
    concept IsTexture = requires(T texture){
        {texture.GetSampler()} -> std::same_as<vk::Sampler>;
        {texture.GetImageView()} -> std::same_as<vk::ImageView>;
        {texture.GetTextureIndex()} -> std::same_as<uint32_t>;
    };

    struct DescriptorImageInfo{
        constexpr DescriptorImageInfo() = default;

        constexpr DescriptorImageInfo(const uint32_t binding, const uint32_t descriptorCount, const vk::DescriptorType descriptorType, const IsTexture auto& texture)
            : m_binding{binding}, m_descriptorCount{descriptorCount}, m_descriptorArrayIndex{texture.GetTextureIndex()}, m_descriptorType{descriptorType}, m_imageInfo{texture.GetSampler(), texture.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal}
        {
            std::println("Update Texture Index: {}", m_descriptorArrayIndex);
        }

        uint32_t m_binding{};
        uint32_t m_descriptorCount{};
        uint32_t m_descriptorArrayIndex{};
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