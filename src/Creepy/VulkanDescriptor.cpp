#include <print>
#include <Creepy/VulkanDescriptor.hpp>
#include <Creepy/Texture.hpp>

namespace Creepy{

    void DescriptorSetBuilder::AddBinding(const uint32_t binding, const uint32_t descriptorCount, vk::DescriptorType type, vk::ShaderStageFlags shaderStage) {
        m_bindings.emplace_back(binding, type, descriptorCount, shaderStage);
    }

    void DescriptorSetBuilder::AddBindingWithFlag(const uint32_t binding, const uint32_t descriptorCount, vk::DescriptorType type, vk::ShaderStageFlags shaderStage, vk::DescriptorBindingFlags bindingFlags) {
        m_bindingFlags.push_back(bindingFlags);
        this->AddBinding(binding, descriptorCount, type, shaderStage);
    }

    vk::DescriptorSetLayout DescriptorSetBuilder::BuildDescriptorLayout(const vk::Device device) {
        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.flags = vk::DescriptorSetLayoutCreateFlags{};
        layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
        layoutInfo.pBindings = m_bindings.data();

        auto res = device.createDescriptorSetLayout(layoutInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create DescriptorSet Layout");
        }

        m_descriptorSetLayout = res.value;

        return m_descriptorSetLayout;
    }

    vk::DescriptorSetLayout DescriptorSetBuilder::BuildDescriptorLayoutWithFlags(const vk::Device device) {
        // Ensure we call right API
        assert(m_bindings.size() == m_bindingFlags.size());

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;
        layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
        layoutInfo.pBindings = m_bindings.data();

        vk::DescriptorSetLayoutBindingFlagsCreateInfo layoutBindingInfo{};
        layoutBindingInfo.bindingCount = static_cast<uint32_t>(m_bindingFlags.size());
        layoutBindingInfo.pBindingFlags = m_bindingFlags.data();

        layoutInfo.pNext = &layoutBindingInfo;

        auto res = device.createDescriptorSetLayout(layoutInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create DescriptorSet Layout");
        }

        m_descriptorSetLayout = res.value;
        
        return m_descriptorSetLayout;
    }

    DescriptorSet DescriptorSetBuilder::AllocateDescriptorSet(const vk::Device device, const vk::DescriptorPool descriptorPool){
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_descriptorSetLayout;

        auto res = device.allocateDescriptorSets(allocInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Alloc DescriptorSet");
        }

        return {res.value.at(0), m_descriptorSetLayout};
    }

    DescriptorSet DescriptorSetBuilder::AllocateDescriptorSetWithFlags(const vk::Device device, const vk::DescriptorPool descriptorPool) {
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_descriptorSetLayout;

        // constexpr std::array<uint32_t, 1> ar{
        //     1u
        // };

        // Only if we want create resources Unbound Array with size
        // vk::DescriptorSetVariableDescriptorCountAllocateInfo varAllocInfo{};
        // varAllocInfo.descriptorSetCount = static_cast<uint32_t>(ar.size());
        // varAllocInfo.pDescriptorCounts = ar.data();

        // allocInfo.pNext = &varAllocInfo;

        auto res = device.allocateDescriptorSets(allocInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Alloc DescriptorSet");
        }

        return {res.value.at(0), m_descriptorSetLayout};
    }


    void DescriptorSetWriter::AddBufferBinding(const vk::DescriptorSet descriptorSet, const DescriptorBufferInfoBuilder& bufferInfos) {
        
        for(auto&& bufferInfo : bufferInfos.m_descriptorBufferInfos){
            vk::WriteDescriptorSet writer{};
            writer.descriptorCount = bufferInfo.m_descriptorCount;
            writer.pBufferInfo = &bufferInfo.m_bufferInfo;
            writer.descriptorType = bufferInfo.m_descriptorType;
            writer.dstBinding = bufferInfo.m_binding;
            writer.dstSet = descriptorSet;
            m_writers.push_back(std::move(writer));
        }
    }

    void DescriptorSetWriter::AddImageBinding(const vk::DescriptorSet descriptorSet, const DescriptorImageInfoBuilder& imageInfos) {
        for(auto&& imageInfo : imageInfos.m_descriptorImageInfos){
            vk::WriteDescriptorSet writer{};
            writer.descriptorCount = imageInfo.m_descriptorCount;
            writer.pImageInfo = &imageInfo.m_imageInfo;
            writer.descriptorType = imageInfo.m_descriptorType;
            writer.dstBinding = imageInfo.m_binding;
            writer.dstSet = descriptorSet;
            writer.dstArrayElement = imageInfo.m_descriptorArrayIndex;

            m_writers.push_back(std::move(writer));
        }
    }

    void DescriptorSetWriter::UpdateDescriptorSets(const vk::Device device) {
        device.updateDescriptorSets(m_writers, nullptr);
    }
}