#pragma once

#include <print>
#include <vulkan/vulkan.hpp>
#include <vkmemoryalloc/vk_mem_alloc.hpp>
#include "VulkanAllocator.hpp"
#include "Vertex.hpp"

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
            void UploadData(const void* data, size_t dataSizeInByte);

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

            void UploadData(const vk::CommandBuffer commandBuffer, const void *data, size_t dataSizeInByte);

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

        private:
            vk::Buffer m_buffer;
            vk::BufferView m_bufferView;
            vma::Allocation m_bufferLoc;
            uint64_t m_bufferSize;
    };


    template <typename T, vk::BufferUsageFlagBits... bufferUsages>
    class BufferWrapperNoView{
        public:
            using valueType_t = T;
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

            void UploadData(std::span<const T> data){
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

        private:
            //TODO: Maybe change buffer to template?
            Buffer<BufferType::HOST_VISIBLE> m_buffer;
    };

    using VertexBuffer = BufferWrapperNoView<Vertex, vk::BufferUsageFlagBits::eVertexBuffer>;
    using IndexBuffer = BufferWrapperNoView<uint32_t, vk::BufferUsageFlagBits::eIndexBuffer>;
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
void Creepy::Buffer<Creepy::BufferType::HOST_VISIBLE>::UploadData(const void* data, size_t dataSizeInByte);

template <>
void Creepy::Buffer<Creepy::BufferType::HOST_COHERENT>::UploadData(const void* data, size_t dataSizeInByte);