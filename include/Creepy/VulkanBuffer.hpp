#pragma once

#include <print>
#include <vulkan/vulkan.hpp>
#include <vkmemoryalloc/vk_mem_alloc.hpp>
#include "VulkanAllocator.hpp"
#include "Vertex.hpp"
#include "Uniform.hpp"

namespace Creepy {

    enum class BufferType : uint32_t {
        DEVICE_LOCAL, HOST_VISIBLE, HOST_COHERENT
    };

    template <BufferType bufferType = BufferType::HOST_VISIBLE>
    class Buffer
    {
        public:
            static constexpr auto valueType = bufferType;

            Buffer() = default;
            
            Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage){
                std::println("Hum Something Wrong");
                assert(false);
            }

            Buffer(const vk::Device device, uint64_t bufferSize, vk::BufferUsageFlags bufferUsage){
                std::println("Hum Something Wrong");
                assert(false);
            }

            //TODO: Handle uint64_t dataSize
            void UploadData(const void* data, size_t dataSizeInByte) const;

            void UploadData(const void* data, size_t dataSizeInByte, uint64_t offset) const;

            void Destroy(const vk::Device device) const {
                // device.destroyBufferView(m_bufferView);
                VulkanAllocator::BufferAllocator.destroyBuffer(m_buffer, m_bufferLoc);
            }

            vk::Buffer GetBuffer() const {
                return m_buffer;
            }

            vk::BufferView GetBufferView() const {
                return m_bufferView;
            }

            uint64_t GetBufferSize() const {
                return m_bufferSize;
            }

            uint64_t GetBufferOffset() const{
                return m_bufferInfo.offset;
            }

            // Note(Creepy): Total Size
            uint64_t GetBufferCapacity() const {
                return m_bufferInfo.size;
            }

        private:
            vk::Buffer m_buffer;
            vk::BufferView m_bufferView;
            vma::Allocation m_bufferLoc;

            // TODO: Maybe use another way to reduce buffer size
            vma::AllocationInfo m_bufferInfo;
            uint64_t m_bufferSize;
    };


    template <>
    class Buffer<BufferType::DEVICE_LOCAL>
    {
        public:
            static constexpr auto valueType = BufferType::DEVICE_LOCAL;

            Buffer() = default;

            Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage);

            Buffer(const vk::Device device, uint64_t bufferSize, vk::BufferUsageFlags bufferUsage);

            void UploadData(const vk::Device device, const vk::CommandBuffer commandBuffer, const void *data, size_t dataSizeInByte) const;
            
            void UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, const void *data, size_t dataSizeInByte) const;

            void Destroy(const vk::Device device) const;

            vk::Buffer GetBuffer() const {
                return m_buffer;
            }

            vk::BufferView GetBufferView() const {
                return m_bufferView;
            }

            uint64_t GetBufferSize() const {
                return m_bufferSize;
            }

            uint64_t GetBufferOffset() const;

            uint64_t GetBufferCapacity() const;

        private:
            vk::Buffer m_buffer;
            vk::BufferView m_bufferView;
            vma::Allocation m_bufferLoc;
            uint64_t m_bufferSize;
    };


    template <BufferType bufferType, typename T, vk::BufferUsageFlagBits... bufferUsages>
    class BufferWrapperNoView{
        public:
            using valueType_t = T;
            static constexpr auto bufferType_v = bufferType;
            static constexpr auto bufferUsageFlags = (bufferUsages | ...);

            BufferWrapperNoView() = default;

            BufferWrapperNoView(const vk::Device device, uint64_t bufferSize)
                : m_buffer{device, bufferSize, (bufferUsages | ...)}
            {
                
            }

            BufferWrapperNoView(const vk::Device device, std::span<const T> data)
                : m_buffer{device, data.size() * sizeof(T), (bufferUsages | ...)}
            {
                m_buffer.UploadData(data.data(), data.size() * sizeof(T));
            }
            
            // For Upload Array Of Buffer Data
            void UploadData(std::span<const T> data) const {
                m_buffer.UploadData(data.data(), data.size() * sizeof(T));
            }

            // For Upload Single Data Object
            void UploadData(const T& data) const {
                m_buffer.UploadData(&data, sizeof(T));
            }

            void Destroy(const vk::Device device) const {
                m_buffer.Destroy(device);
            }

            vk::Buffer GetBuffer() const {
                return m_buffer.GetBuffer();
            }

            uint64_t GetBufferSize() const {
                return m_buffer.GetBufferSize();
            }

            uint64_t GetBufferOffset() const {
                return m_buffer.GetBufferOffset();
            }

        private:
            Buffer<bufferType> m_buffer;
    };

    template <typename T, vk::BufferUsageFlagBits... bufferUsages>
    class BufferWrapperNoView<BufferType::DEVICE_LOCAL, T, bufferUsages...>{
        public:
            using valueType_t = T;
            static constexpr auto bufferType_v = BufferType::DEVICE_LOCAL;
            static constexpr auto bufferUsageFlags = (bufferUsages | ...);

            BufferWrapperNoView() = default;

            BufferWrapperNoView(const vk::Device device, uint64_t bufferSize)
                : m_buffer{device, bufferSize, (bufferUsages | ...)}
            {
                
            }

            void UploadData(const vk::Device device, const vk::CommandBuffer commandBuffer, std::span<const T> data) const {
                m_buffer.UploadData(device, commandBuffer, data.data(), data.size() * sizeof(T));
            }

            void UploadData(const vk::Device device, const vk::CommandBuffer commandBuffer, const T& data) const {
                m_buffer.UploadData(device, commandBuffer, &data, sizeof(T));
            }

            void UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const T> data) const {
                m_buffer.UploadData(device, commandPool, queue, data.data(), data.size() * sizeof(T));
            }

            void UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, const T& data) const {
                m_buffer.UploadData(device, commandPool, queue, &data, sizeof(T));
            }

            void Destroy(const vk::Device device) const {
                m_buffer.Destroy(device);
            }

            vk::Buffer GetBuffer() const {
                return m_buffer.GetBuffer();
            }

            uint64_t GetBufferSize() const {
                return m_buffer.GetBufferSize();
            }

            uint64_t GetBufferOffset() const {
                return m_buffer.GetBufferOffset();
            }

            uint32_t GetBufferCount() const {
                return static_cast<uint32_t>(this->GetBufferSize() / sizeof(T));
            }

        private:
            Buffer<BufferType::DEVICE_LOCAL> m_buffer;
    };

    template <typename T>
    using StagingBuffer = BufferWrapperNoView<BufferType::HOST_VISIBLE, T, vk::BufferUsageFlagBits::eTransferSrc>;

    using InterLeaveVertexBuffer = BufferWrapperNoView<BufferType::DEVICE_LOCAL, VertexInterLeave, vk::BufferUsageFlagBits::eVertexBuffer, vk::BufferUsageFlagBits::eTransferDst>;
    using IndexBuffer = BufferWrapperNoView<BufferType::DEVICE_LOCAL, uint32_t, vk::BufferUsageFlagBits::eIndexBuffer, vk::BufferUsageFlagBits::eTransferDst>;
    
    struct UniformBuffer{
        using TransformBuffer = BufferWrapperNoView<BufferType::HOST_COHERENT, TransformData, vk::BufferUsageFlagBits::eUniformBuffer, vk::BufferUsageFlagBits::eTransferDst>;
        using LightBuffer = BufferWrapperNoView<BufferType::HOST_COHERENT, LightData, vk::BufferUsageFlagBits::eShaderDeviceAddress>;
        TransformBuffer transformBuffer;
        LightBuffer lightBuffer;
        vk::DeviceAddress lightBufferAddress{0};
    };

    using MaterialBuffer = BufferWrapperNoView<BufferType::HOST_COHERENT, MaterialData, vk::BufferUsageFlagBits::eShaderDeviceAddress>;


    struct SeparateVertexBuffer{
        private:
            using Vec3fBuffer = BufferWrapperNoView<BufferType::DEVICE_LOCAL, glm::vec3, vk::BufferUsageFlagBits::eVertexBuffer, vk::BufferUsageFlagBits::eTransferDst>;
            using Vec2fBuffer = BufferWrapperNoView<BufferType::DEVICE_LOCAL, glm::vec2, vk::BufferUsageFlagBits::eVertexBuffer, vk::BufferUsageFlagBits::eTransferDst>;
            using Vec1uiBuffer = BufferWrapperNoView<BufferType::DEVICE_LOCAL, uint32_t, vk::BufferUsageFlagBits::eVertexBuffer, vk::BufferUsageFlagBits::eTransferDst>;
        
        public:
            SeparateVertexBuffer() = default;

            SeparateVertexBuffer(const vk::Device device, const VertexSeparate& vertexSeparate);

            void UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, const VertexSeparate& vertexSeparate);
            
        public:
            Vec3fBuffer PositionBuffer;
            Vec3fBuffer NormalBuffer;
            Vec2fBuffer TexCoordBuffer;
    };
}

// Template Instanciation
template <>
Creepy::Buffer<Creepy::BufferType::HOST_VISIBLE>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage);

template <>
Creepy::Buffer<Creepy::BufferType::HOST_COHERENT>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage);

template <>
Creepy::Buffer<Creepy::BufferType::HOST_VISIBLE>::Buffer(const vk::Device device, uint64_t bufferSize, vk::BufferUsageFlags bufferUsage);

template <>
Creepy::Buffer<Creepy::BufferType::HOST_COHERENT>::Buffer(const vk::Device device, uint64_t bufferSize, vk::BufferUsageFlags bufferUsage);


template <>
void Creepy::Buffer<Creepy::BufferType::HOST_VISIBLE>::UploadData(const void* data, size_t dataSizeInByte) const;

template <>
void Creepy::Buffer<Creepy::BufferType::HOST_COHERENT>::UploadData(const void* data, size_t dataSizeInByte) const;

template <>
void Creepy::Buffer<Creepy::BufferType::HOST_VISIBLE>::UploadData(const void* data, size_t dataSizeInByte, uint64_t offset) const;

template <>
void Creepy::Buffer<Creepy::BufferType::HOST_COHERENT>::UploadData(const void* data, size_t dataSizeInByte, uint64_t offset) const;