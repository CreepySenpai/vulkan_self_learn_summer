#include <Creepy/VulkanBuffer.hpp>

namespace Creepy{

    template <>
    Buffer<BufferType::DEVICE_LOCAL>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage)
        : m_bufferSize{bufferSize}
    {
        std::println("Init Device Local");

        vk::BufferCreateInfo info{};
        info.flags = vk::BufferCreateFlags{};
        info.sharingMode = vk::SharingMode::eExclusive;
        info.size = bufferSize;
        info.usage = bufferUsage;
        
        vma::AllocationCreateInfo allocInfo{};
        allocInfo.flags = vma::AllocationCreateFlags{};
        allocInfo.usage = vma::MemoryUsage::eGpuOnly;
        allocInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

        auto res = VulkanAllocator::BufferAllocator.createBuffer(info, allocInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Buffer");
        }

        std::tie(m_buffer, m_bufferLoc) = res.value;

        vk::BufferViewCreateInfo bufferViewInfo{};
        bufferViewInfo.flags = vk::BufferViewCreateFlags{};
        bufferViewInfo.buffer = m_buffer;
        bufferViewInfo.offset = 0;
        bufferViewInfo.format = bufferFormat;
        bufferViewInfo.range = m_bufferSize;
        
        auto viewRes = device.createBufferView(bufferViewInfo);
        
        if(viewRes.result != vk::Result::eSuccess){
            std::println("Failed Create BufferView");
        }

        m_bufferView = viewRes.value;
    }


    template <>
    Buffer<BufferType::HOST_VISIBLE>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage)
        : m_bufferSize{bufferSize}
    {
        std::println("Init Host Visible");

        vk::BufferCreateInfo info{};
        info.flags = vk::BufferCreateFlags{};
        info.sharingMode = vk::SharingMode::eExclusive;
        info.size = bufferSize;
        info.usage = bufferUsage;
        
        vma::AllocationCreateInfo allocInfo{};
        allocInfo.flags = vma::AllocationCreateFlags{};
        allocInfo.usage = vma::MemoryUsage::eCpuToGpu;
        allocInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;

        auto res = VulkanAllocator::BufferAllocator.createBuffer(info, allocInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Buffer");
        }

        std::tie(m_buffer, m_bufferLoc) = res.value;

        vk::BufferViewCreateInfo bufferViewInfo{};
        bufferViewInfo.flags = vk::BufferViewCreateFlags{};
        bufferViewInfo.buffer = m_buffer;
        bufferViewInfo.offset = 0;
        bufferViewInfo.format = bufferFormat;
        bufferViewInfo.range = m_bufferSize;
        
        auto viewRes = device.createBufferView(bufferViewInfo);
        
        if(viewRes.result != vk::Result::eSuccess){
            std::println("Failed Create BufferView");
        }

        m_bufferView = viewRes.value;
    }

    template <>
    Buffer<BufferType::HOST_COHERENT>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage)
        : m_bufferSize{bufferSize}
    {
        std::println("Init Host Coherent");
        vk::BufferCreateInfo info{};
        info.flags = vk::BufferCreateFlags{};
        info.sharingMode = vk::SharingMode::eExclusive;
        info.size = bufferSize;
        info.usage = bufferUsage;
        
        vma::AllocationCreateInfo allocInfo{};
        allocInfo.flags = vma::AllocationCreateFlags{};
        allocInfo.usage = vma::MemoryUsage::eCpuToGpu;
        allocInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostCoherent;

        auto res = VulkanAllocator::BufferAllocator.createBuffer(info, allocInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Buffer");
        }

        std::tie(m_buffer, m_bufferLoc) = res.value;

        vk::BufferViewCreateInfo bufferViewInfo{};
        bufferViewInfo.flags = vk::BufferViewCreateFlags{};
        bufferViewInfo.buffer = m_buffer;
        bufferViewInfo.offset = 0;
        bufferViewInfo.format = bufferFormat;
        bufferViewInfo.range = m_bufferSize;
        
        auto viewRes = device.createBufferView(bufferViewInfo);
        
        if(viewRes.result != vk::Result::eSuccess){
            std::println("Failed Create BufferView");
        }

        m_bufferView = viewRes.value;
    }


    template <>
    void Buffer<BufferType::DEVICE_LOCAL>::UploadData(const void* data, uint64_t dataSize, const vk::CommandBuffer commandBuffer) {
        if(commandBuffer == nullptr){
            std::println("Require Command Buffer");
        }
    }

    template <>
    void Buffer<BufferType::HOST_VISIBLE>::UploadData(const void* data, uint64_t dataSize, const vk::CommandBuffer commandBuffer) {
        std::println("Copy Ok");
    }

    template <>
    void Buffer<BufferType::HOST_COHERENT>::UploadData(const void* data, uint64_t dataSize, const vk::CommandBuffer commandBuffer) {
        std::println("Still Ok");
    }
}