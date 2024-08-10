#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

struct DescriptorSet{
    vk::DescriptorSet m_descriptorSet;
    vk::DescriptorSetLayout m_descriptorSetLayout;
};

class DescriptorSetBuilder{
    public:
        void AddBinding(const uint32_t binding, vk::DescriptorType type);
        
        void BuildDesciptorLayout(const vk::Device device, vk::ShaderStageFlags shaderStage);

        DescriptorSet AllocateDescriptorSet(const vk::Device device, const vk::DescriptorPool descriptorPool);
    private:
        std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
        vk::DescriptorSetLayout m_descriptorSetLayout{};
};