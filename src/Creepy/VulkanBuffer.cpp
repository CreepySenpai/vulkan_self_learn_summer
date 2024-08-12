#include <cstring>
#include <Creepy/VulkanBuffer.hpp>

namespace Creepy{

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

        // vk::BufferViewCreateInfo bufferViewInfo{};
        // bufferViewInfo.flags = vk::BufferViewCreateFlags{};
        // bufferViewInfo.buffer = m_buffer;
        // bufferViewInfo.offset = 0;
        // bufferViewInfo.format = bufferFormat;
        // bufferViewInfo.range = m_bufferSize;
        
        // auto viewRes = device.createBufferView(bufferViewInfo);
        
        // if(viewRes.result != vk::Result::eSuccess){
        //     std::println("Failed Create BufferView");
        // }

        // m_bufferView = viewRes.value;
    }

    void Buffer<BufferType::DEVICE_LOCAL>::UploadData(const vk::CommandBuffer commandBuffer, const void* data, size_t dataSizeInByte) {
        if(commandBuffer == nullptr){
            std::println("Require Command Buffer");
        }
    }

    void Buffer<BufferType::DEVICE_LOCAL>::Destroy(const vk::Device device)
    {
        // device.destroyBufferView(m_bufferView);
        VulkanAllocator::ImageAllocator.destroyBuffer(m_buffer, m_bufferLoc);
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
        allocInfo.flags = vma::AllocationCreateFlagBits::eMapped;
        allocInfo.usage = vma::MemoryUsage::eCpuToGpu;
        allocInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostVisible;

        auto res = VulkanAllocator::BufferAllocator.createBuffer(info, allocInfo);
        
        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Buffer");
        }

        std::tie(m_buffer, m_bufferLoc) = res.value;

        m_bufferInfo = VulkanAllocator::BufferAllocator.getAllocationInfo(m_bufferLoc);

        // vk::BufferViewCreateInfo bufferViewInfo{};
        // bufferViewInfo.flags = vk::BufferViewCreateFlags{};
        // bufferViewInfo.buffer = m_buffer;
        // bufferViewInfo.offset = 0;
        // bufferViewInfo.format = bufferFormat;
        // bufferViewInfo.range = m_bufferSize;
        
        // auto viewRes = device.createBufferView(bufferViewInfo);
        
        // if(viewRes.result != vk::Result::eSuccess){
        //     std::println("Failed Create BufferView");
        // }

        // m_bufferView = viewRes.value;
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
        allocInfo.flags = vma::AllocationCreateFlagBits::eMapped;
        allocInfo.usage = vma::MemoryUsage::eCpuToGpu;
        allocInfo.requiredFlags = vk::MemoryPropertyFlagBits::eHostCoherent;

        auto res = VulkanAllocator::BufferAllocator.createBuffer(info, allocInfo);
        // VulkanAllocator::BufferAllocator.createBuffer()
        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Buffer");
        }

        std::tie(m_buffer, m_bufferLoc) = res.value;

        m_bufferInfo = VulkanAllocator::BufferAllocator.getAllocationInfo(m_bufferLoc);

        // vk::BufferViewCreateInfo bufferViewInfo{};
        // bufferViewInfo.flags = vk::BufferViewCreateFlags{};
        // bufferViewInfo.buffer = m_buffer;
        // bufferViewInfo.offset = 0;
        // bufferViewInfo.format = bufferFormat;
        // bufferViewInfo.range = m_bufferSize;
        
        // auto viewRes = device.createBufferView(bufferViewInfo);
        
        // if(viewRes.result != vk::Result::eSuccess){
        //     std::println("Failed Create BufferView");
        // }

        // m_bufferView = viewRes.value;
    }

    template <>
    void Buffer<BufferType::HOST_VISIBLE>::UploadData(const void* data, size_t dataSizeInByte) {
        std::println("Host Vi: {}, {}, {}", m_bufferInfo.memoryType, m_bufferInfo.offset, m_bufferInfo.size);
        
        if(dataSizeInByte > static_cast<size_t>(m_bufferInfo.size)){
            std::println("Data too big");
            return;
        }

        std::memcpy(m_bufferInfo.pMappedData, data, dataSizeInByte);
    }

    template <>
    void Buffer<BufferType::HOST_COHERENT>::UploadData(const void* data, size_t dataSizeInByte) {
        std::println("Host Co: {}, {}, {}", m_bufferInfo.memoryType, m_bufferInfo.offset, m_bufferInfo.size);
        if(dataSizeInByte > static_cast<size_t>(m_bufferInfo.size)){
            std::println("Data too big");
            return;
        }

        std::memcpy(m_bufferInfo.pMappedData, data, dataSizeInByte);
    }
}