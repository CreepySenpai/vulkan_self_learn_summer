#include <print>
#include <Creepy/VulkanDescriptor.hpp>
#include <Creepy/Texture.hpp>

namespace Creepy{
    void DescriptorSetBuilder::AddBinding(const uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags shaderStage) {
        auto& bindingType = m_bindings.emplace_back(binding, type, 1);
        bindingType.stageFlags |= shaderStage;
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

        auto supportInfo = device.getDescriptorSetLayoutSupport(layoutInfo);
        std::println("DescriptorSetLayoutInfo: {}", supportInfo.supported);
        

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

            m_writers.push_back(std::move(writer));
        }
    }

    void DescriptorSetWriter::UpdateDescriptorSets(const vk::Device device) {
        device.updateDescriptorSets(m_writers, nullptr);
    }
}