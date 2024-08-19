#include <print>
#include <Creepy/VulkanDescriptor.hpp>
#include <Creepy/Texture.hpp>

namespace Creepy{
    void DescriptorSetBuilder::AddBinding(const uint32_t binding, vk::DescriptorType type) {
        m_bindings.emplace_back(binding, type, 1);
    }
            
    void DescriptorSetBuilder::BuildDescriptorLayout(const vk::Device device, vk::ShaderStageFlags shaderStage) {
        for(auto&& binding : m_bindings){
            binding.stageFlags |= shaderStage;
        }

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.flags = vk::DescriptorSetLayoutCreateFlags{};
        layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
        layoutInfo.pBindings = m_bindings.data();

        auto res = device.createDescriptorSetLayout(layoutInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create DescriptorSet Layout");
        }

        m_descriptorSetLayout = res.value;
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


    //////////////////////////////////////////////////////////////////////////////

    void DescriptorSetWriter::AddBufferBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, vk::DescriptorType descriptorType, const vk::Buffer buffer, uint64_t bufferSize) {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.offset = 0;
        bufferInfo.buffer = buffer;
        bufferInfo.range = bufferSize;
        
        
        vk::WriteDescriptorSet writer{};
        writer.descriptorCount = 1;
        writer.descriptorType = descriptorType;
        writer.dstBinding = binding;
        writer.dstSet = descriptorSet;
        writer.pBufferInfo = &bufferInfo;

        m_writers.push_back(std::move(writer));
    }

    void DescriptorSetWriter::AddImageBinding(const uint32_t binding, const vk::DescriptorSet descriptorSet, vk::DescriptorType descriptorType, const Texture& texture) {
        vk::DescriptorImageInfo textureInfo{};
        textureInfo.imageView = texture.GetImageView();
        textureInfo.sampler = texture.GetSampler();

        //TODO: Storage Texture Layout
        textureInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        
        
        vk::WriteDescriptorSet writer{};
        writer.descriptorCount = 1;
        writer.descriptorType = descriptorType;
        writer.dstBinding = binding;
        writer.dstSet = descriptorSet;
        writer.pImageInfo = &textureInfo;

        m_writers.push_back(std::move(writer));
    }

    void DescriptorSetWriter::UpdateDescriptorSets(const vk::Device device) {
        device.updateDescriptorSets(m_writers, nullptr);
    }
}