#include <print>
#include <Creepy/VulkanDescriptor.hpp>


void DescriptorSetBuilder::AddBinding(const uint32_t binding, vk::DescriptorType type) {
    m_bindings.emplace_back(binding, type, 1);
}
        
void DescriptorSetBuilder::BuildDesciptorLayout(const vk::Device device, vk::ShaderStageFlags shaderStage) {
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