#pragma once

#include <print>
#include <vulkan/vulkan.hpp>
#include <vkmemoryalloc/vk_mem_alloc.hpp>
#include "VulkanAllocator.hpp"

namespace Creepy {

    enum class BufferType : uint32_t {
        DEVICE_LOCAL, HOST_VISIBLE, HOST_COHERENT
    };

    template <BufferType bufferType = BufferType::DEVICE_LOCAL>
    class Buffer
    {
        public:
            static constexpr auto valueType = bufferType;

            Buffer() = default;
            Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage){
                std::println("Hum Something Wrong");
                assert(false);
            }

            void UploadData(const void* data, uint64_t dataSize, const vk::CommandBuffer commandBuffer = nullptr);

            void Destroy(const vk::Device device){
                device.destroyBufferView(m_bufferView);
                VulkanAllocator::ImageAllocator.destroyBuffer(m_buffer, m_bufferLoc);
            }
        private:
            vk::Buffer m_buffer;
            vk::BufferView m_bufferView;
            vma::Allocation m_bufferLoc;
            uint64_t m_bufferSize;
    };
}

// Template Instanciation
template <>
Creepy::Buffer<Creepy::BufferType::DEVICE_LOCAL>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage);

template <>
Creepy::Buffer<Creepy::BufferType::HOST_VISIBLE>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage);

template <>
Creepy::Buffer<Creepy::BufferType::HOST_COHERENT>::Buffer(const vk::Device device, uint64_t bufferSize, vk::Format bufferFormat, vk::BufferUsageFlags bufferUsage);




template <>
void Creepy::Buffer<Creepy::BufferType::DEVICE_LOCAL>::UploadData(const void* data, uint64_t dataSize, const vk::CommandBuffer commandBuffer);

template <>
void Creepy::Buffer<Creepy::BufferType::HOST_VISIBLE>::UploadData(const void* data, uint64_t dataSize, const vk::CommandBuffer commandBuffer);

template <>
void Creepy::Buffer<Creepy::BufferType::HOST_COHERENT>::UploadData(const void* data, uint64_t dataSize, const vk::CommandBuffer commandBuffer);