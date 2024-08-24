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

            void Destroy(const vk::Device device) const {
                // device.destroyBufferView(m_bufferView);
                VulkanAllocator::ImageAllocator.destroyBuffer(m_buffer, m_bufferLoc);
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

        private:
            vk::Buffer m_buffer;
            vk::BufferView m_bufferView;
            vma::Allocation m_bufferLoc;
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

            void UploadData(std::span<const T> data) const {
                m_buffer.UploadData(data.data(), data.size() * sizeof(T));
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

            void UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const T> data) const {
                m_buffer.UploadData(device, commandPool, queue, data.data(), data.size() * sizeof(T));
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

    using VertexBuffer = BufferWrapperNoView<BufferType::DEVICE_LOCAL, Vertex, vk::BufferUsageFlagBits::eVertexBuffer, vk::BufferUsageFlagBits::eTransferDst>;
    using IndexBuffer = BufferWrapperNoView<BufferType::DEVICE_LOCAL, uint32_t, vk::BufferUsageFlagBits::eIndexBuffer, vk::BufferUsageFlagBits::eTransferDst>;
    using UniformBuffer = BufferWrapperNoView<BufferType::HOST_COHERENT, UniformData, vk::BufferUsageFlagBits::eUniformBuffer, vk::BufferUsageFlagBits::eTransferDst>;
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