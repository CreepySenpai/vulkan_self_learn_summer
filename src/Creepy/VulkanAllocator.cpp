#include <print>
#include <Creepy/VulkanAllocator.hpp>

namespace Creepy {

    void InitAllocator(const vk::Instance instance, const vk::Device device, const vk::PhysicalDevice physicalDev) {

        vma::AllocatorCreateInfo info{};
        info.flags = vma::AllocatorCreateFlags{};
        info.device = device;
        info.physicalDevice = physicalDev;
        info.vulkanApiVersion = vk::ApiVersion13;
        info.instance = instance;

        vma::VulkanFunctions vulkanFunc{};
        vulkanFunc.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vulkanFunc.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        vulkanFunc.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
        vulkanFunc.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;

        info.pVulkanFunctions = &vulkanFunc;

        auto imageAllocRes = vma::createAllocator(info);

        if(imageAllocRes.result != vk::Result::eSuccess){
            std::println("Failed create ImageAllocator");
        }
        
        VulkanAllocator::ImageAllocator = imageAllocRes.value;
        
        info.flags = vma::AllocatorCreateFlagBits::eBufferDeviceAddress;
        auto bufferAllocRes = vma::createAllocator(info);

        if(bufferAllocRes.result != vk::Result::eSuccess){
            std::println("Failed create BufferAllocator");
        }

        VulkanAllocator::BufferAllocator = bufferAllocRes.value;
    }

    void DestroyAllocator() {
        std::println("Destroy Alloc");
        VulkanAllocator::ImageAllocator.destroy();
        VulkanAllocator::BufferAllocator.destroy();
    }

}