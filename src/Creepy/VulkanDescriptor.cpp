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


    void DescriptorSetWriter::AddBufferBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, const DescriptorBufferInfoBuilder& bufferInfos) {
        vk::WriteDescriptorSet writer{};
        writer.descriptorCount = bufferInfos.m_descriptorBufferInfo.size();
        writer.pBufferInfo = bufferInfos.m_descriptorBufferInfo.data();
        writer.descriptorType = vk::DescriptorType::eUniformBuffer;
        writer.dstBinding = binding;
        writer.dstSet = descriptorSet;
    
        m_writers.push_back(std::move(writer));
    }

    void DescriptorSetWriter::AddImageBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, const DescriptorImageInfoBuilder& imageInfos) {
        vk::WriteDescriptorSet writer{};
        //FIXME: this count for array of resources in shader
        writer.descriptorCount = imageInfos.m_descriptorImageInfos.size();
        writer.pImageInfo = imageInfos.m_descriptorImageInfos.data();
        writer.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        writer.dstBinding = binding;
        writer.dstSet = descriptorSet;

        m_writers.push_back(std::move(writer));
    }

    void DescriptorSetWriter::UpdateDescriptorSets(const vk::Device device) {
        device.updateDescriptorSets(m_writers, nullptr);
    }
}