#include <cstring>
#include <Creepy/VulkanBuffer.hpp>
#include <Creepy/Vertex.hpp>
#include <Creepy/VulkanUtils.hpp>

namespace Creepy{

    // TODO: Move to utility, maybe more argument?
    vk::BufferCreateInfo createBufferInfo(uint64_t bufferSize, vk::BufferUsageFlags bufferUsage){

        vk::BufferCreateInfo info{};
        info.flags = vk::BufferCreateFlags{};
        info.sharingMode = vk::SharingMode::eExclusive;
        info.size = bufferSize;
        info.usage = bufferUsage;
        
        return info;
    }

    vma::AllocationCreateInfo createBufferAllocationInfo(vma::AllocationCreateFlags flags, vma::MemoryUsage memoryUsage, vk::MemoryPropertyFlags requiredFlags){
        vma::AllocationCreateInfo allocInfo{};
        allocInfo.flags = flags;
        allocInfo.usage = memoryUsage;
        allocInfo.requiredFlags = requiredFlags;

        return allocInfo;
    }

    vk::DescriptorBufferInfo createDescriptorBuffer(const vk::Buffer buffer, const vma::AllocationInfo& bufferAllocInfo, uint64_t bufferSize){
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = bufferAllocInfo.offset;
        bufferInfo.range = bufferSize;
        
        return bufferInfo;
    }

    ///////////////////////////////////////////////////////////////////////////////////

    Buffer<BufferType::DEVICE_LOCAL>::Buffer(const vk::Device device, uint64_t bufferSize, vk::BufferUsageFlags bufferUsage)
        : m_bufferSize{bufferSize}
    {
        std::println("Init Device Local");
        
        const vk::BufferCreateInfo info{createBufferInfo(bufferSize, bufferUsage)};
        
        const vma::AllocationCreateInfo allocInfo{createBufferAllocationInfo(vma::AllocationCreateFlags{}, vma::MemoryUsage::eGpuOnly, vk::MemoryPropertyFlagBits::eDeviceLocal)};

        auto res = VulkanAllocator::BufferAllocator.createBuffer(info, allocInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Buffer");
        }
        
        std::tie(m_buffer, m_bufferLoc) = res.value;
    }

    Buffer<BufferType::DEVICE_LOCAL>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage)
        : Buffer<BufferType::DEVICE_LOCAL>{device, bufferSize, bufferUsage}
    {
        //TODO: Create BufferView
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

    void Buffer<BufferType::DEVICE_LOCAL>::UploadData(const vk::Device device, const vk::CommandBuffer commandBuffer, const void* data, size_t dataSizeInByte) const {
        std::println("StartCopy");
        const Buffer<BufferType::HOST_VISIBLE> stagingBuffer{device, dataSizeInByte, vk::BufferUsageFlagBits::eTransferSrc};
        stagingBuffer.UploadData(data, dataSizeInByte);
        
        // vk::CopyBuff
        const vk::BufferCopy copyInfo{0, 0, dataSizeInByte};

        commandBuffer.copyBuffer(stagingBuffer.GetBuffer(), m_buffer, copyInfo);

        stagingBuffer.Destroy(device);
    }

    void Buffer<BufferType::DEVICE_LOCAL>::UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, const void *data, size_t dataSizeInByte) const {
        std::println("StartCopy");

        const Buffer<BufferType::HOST_VISIBLE> stagingBuffer{device, dataSizeInByte, vk::BufferUsageFlagBits::eTransferSrc};
        stagingBuffer.UploadData(data, dataSizeInByte);

        auto tempCommandBuffer = BeginOneTimeCommandBuffer(device, commandPool);

        const vk::BufferCopy copyInfo{0, 0, dataSizeInByte};

        tempCommandBuffer.copyBuffer(stagingBuffer.GetBuffer(), m_buffer, copyInfo);

        EndOneTimeCommandBuffer(device, commandPool, tempCommandBuffer, queue);

        stagingBuffer.Destroy(device);
    }

    void Buffer<BufferType::DEVICE_LOCAL>::Destroy(const vk::Device device) const
    {
        // device.destroyBufferView(m_bufferView);
        VulkanAllocator::BufferAllocator.destroyBuffer(m_buffer, m_bufferLoc);
    }

    uint64_t Buffer<BufferType::DEVICE_LOCAL>::GetBufferOffset() const {
        return VulkanAllocator::BufferAllocator.getAllocationInfo(m_bufferLoc).offset;
    }

    uint64_t Buffer<BufferType::DEVICE_LOCAL>::GetBufferCapacity() const {
        return VulkanAllocator::BufferAllocator.getAllocationInfo(m_bufferLoc).size;
    }

    ////////////////////////////////////////////////////////////////

    template <>
    Buffer<BufferType::HOST_VISIBLE>::Buffer(const vk::Device device, uint64_t bufferSize, vk::BufferUsageFlags bufferUsage) 
        : m_bufferSize{bufferSize}
    {
        std::println("Init Host Visible");

        const vk::BufferCreateInfo info{createBufferInfo(bufferSize, bufferUsage)};
        
        const vma::AllocationCreateInfo allocInfo{createBufferAllocationInfo(vma::AllocationCreateFlagBits::eMapped, vma::MemoryUsage::eCpuToGpu, vk::MemoryPropertyFlagBits::eHostVisible)};

        auto res = VulkanAllocator::BufferAllocator.createBuffer(info, allocInfo);
        
        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Buffer");
        }

        std::tie(m_buffer, m_bufferLoc) = res.value;
        
        m_bufferInfo = VulkanAllocator::BufferAllocator.getAllocationInfo(m_bufferLoc);
    }

    template <>
    Buffer<BufferType::HOST_COHERENT>::Buffer(const vk::Device device, uint64_t bufferSize, vk::BufferUsageFlags bufferUsage)
        : m_bufferSize{bufferSize}
    {
        std::println("Init Host Coherent");
        const vk::BufferCreateInfo info{createBufferInfo(bufferSize, bufferUsage)};
        
        const vma::AllocationCreateInfo allocInfo{createBufferAllocationInfo(vma::AllocationCreateFlagBits::eMapped, vma::MemoryUsage::eCpuToGpu, vk::MemoryPropertyFlagBits::eHostCoherent)};

        auto res = VulkanAllocator::BufferAllocator.createBuffer(info, allocInfo);
        
        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Buffer");
        }

        std::tie(m_buffer, m_bufferLoc) = res.value;

        m_bufferInfo = VulkanAllocator::BufferAllocator.getAllocationInfo(m_bufferLoc);
    }

    template <>
    Buffer<BufferType::HOST_VISIBLE>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage)
        : Buffer<BufferType::HOST_VISIBLE>{device, bufferSize, bufferUsage}
    {
        // TODO: Create Buffer View
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
        : Buffer<BufferType::HOST_COHERENT>{device, bufferSize, bufferUsage}
    {
        // TODO: Create Buffer View
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
    void Buffer<BufferType::HOST_VISIBLE>::UploadData(const void* data, size_t dataSizeInByte) const {
        // std::println("Host Vi: {}, {}, {}", m_bufferInfo.memoryType, m_bufferInfo.offset, m_bufferInfo.size);
        
        if(dataSizeInByte > static_cast<size_t>(m_bufferInfo.size)){
            std::println("Data too big");
            return;
        }

        // std::println("Buffer Address: {}", (uint64_t)m_bufferInfo.pMappedData);
        std::memcpy(m_bufferInfo.pMappedData, data, dataSizeInByte);
    }

    template <>
    void Buffer<BufferType::HOST_COHERENT>::UploadData(const void* data, size_t dataSizeInByte) const {
        // std::println("Host Co: {}, {}, {}", m_bufferInfo.memoryType, m_bufferInfo.offset, m_bufferInfo.size);
        if(dataSizeInByte > static_cast<size_t>(m_bufferInfo.size)){
            std::println("Data too big");
            return;
        }
        // std::println("Buffer Address: {}", (uint64_t)m_bufferInfo.pMappedData);
        std::memcpy(m_bufferInfo.pMappedData, data, dataSizeInByte);
    }

    template <>
    void Buffer<BufferType::HOST_VISIBLE>::UploadData(const void* data, size_t dataSizeInByte, uint64_t offset) const {
        if(dataSizeInByte > static_cast<size_t>(m_bufferInfo.size)){
            std::println("Data too big");
            return;
        }
        
        auto newMapped = static_cast<uint8_t*>(m_bufferInfo.pMappedData) + offset;
        std::memcpy(static_cast<void*>(newMapped), data, dataSizeInByte);
    }

    template <>
    void Buffer<BufferType::HOST_COHERENT>::UploadData(const void* data, size_t dataSizeInByte, uint64_t offset) const {
        if(dataSizeInByte > static_cast<size_t>(m_bufferInfo.size)){
            std::println("Data too big");
            return;
        }

        auto newMapped = static_cast<uint8_t*>(m_bufferInfo.pMappedData) + offset;
        std::memcpy(static_cast<void*>(newMapped), data, dataSizeInByte);
    }
    
}