#pragma once

#include <vulkan/vulkan.hpp>
#include <vkmemoryalloc/vk_mem_alloc.hpp>


namespace Creepy{
    
    void InitAllocator(const vk::Instance instance, const vk::Device device, const vk::PhysicalDevice physicalDev);
    void DestroyAllocator();
    
    struct VulkanAllocator{
        static inline vma::Allocator ImageAllocator;
        static inline vma::Allocator BufferAllocator;
    };
}