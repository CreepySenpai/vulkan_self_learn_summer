#pragma once

#include <print>
#include <filesystem>
#include <fstream>
#include <vulkan/vulkan.hpp>

namespace Creepy{

    static VKAPI_CALL VkBool32 debugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData){
        
        if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            std::println("Error: {}", pCallbackData->pMessage);
        }
        if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
            std::println("Waring: {}", pCallbackData->pMessage);
        }
        if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT){
            std::println("Info: {}", pCallbackData->pMessage);
        }
        if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT){
            std::println("Something: {}", pCallbackData->pMessage);
        }

        return VK_FALSE;
    }

    static VKAPI_CALL void deviceMemoryReportCallback(const VkDeviceMemoryReportCallbackDataEXT* callbackData, void* userData){
        if(callbackData->type == VK_DEVICE_MEMORY_REPORT_EVENT_TYPE_ALLOCATION_FAILED_EXT){
            std::println("Alloc Failed");
        }
    }

    static uint32_t findQueueFamilyIndex(const vk::PhysicalDevice physicalDev, vk::QueueFlags queueType){
        auto queues = physicalDev.getQueueFamilyProperties();

        for(uint32_t queueIndex{}; auto queue : queues){
            if(queue.queueFlags & queueType){
                return queueIndex;
            }
            ++queueIndex;
        }

        return 0;
    }

    static uint32_t findPresentQueueFamilyIndex(const vk::PhysicalDevice physicalDev, const vk::SurfaceKHR surface){
        auto queues = physicalDev.getQueueFamilyProperties();

        for(uint32_t queueIndex{}; auto queue : queues){
            // Check Queue Support Present
            if(physicalDev.getSurfaceSupportKHR(queueIndex, surface).value){
                return queueIndex;
            }
            ++queueIndex;
        }

        return 0;
    }

    static std::vector<char> readShaderSPVFile(const std::filesystem::path& filePath) {
        if(!std::filesystem::exists(filePath)){
            std::println("File Not Exists: {}", filePath.string());
            return {};
        }

        std::ifstream fileIn{filePath, std::ios::binary};
        
        // Create Iter from begin -> end file
        return {std::istreambuf_iterator<char>{fileIn}, std::istreambuf_iterator<char>{}};
    }

    static void imageLayoutTransition(const vk::CommandBuffer commandBuffer, vk::Image image, vk::ImageAspectFlags imageAspect, vk::ImageLayout currentLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccess, vk::AccessFlags2 dstAccess, vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage, uint32_t layerCount = 1u){
        
        vk::ImageMemoryBarrier2 imageBarrier{};
        imageBarrier.image = image;
        imageBarrier.oldLayout = currentLayout;
        imageBarrier.newLayout = newLayout;
        imageBarrier.srcAccessMask = srcAccess;
        imageBarrier.srcStageMask = srcStage;
        imageBarrier.dstAccessMask = dstAccess;
        imageBarrier.dstStageMask = dstStage;
        
        imageBarrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
        imageBarrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;

        imageBarrier.subresourceRange.aspectMask = imageAspect;
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.levelCount = 1;
        imageBarrier.subresourceRange.layerCount = layerCount;

        vk::DependencyInfo depenInfo{};
        depenInfo.dependencyFlags = vk::DependencyFlags{};
        depenInfo.imageMemoryBarrierCount = 1;
        depenInfo.pImageMemoryBarriers = &imageBarrier;

        commandBuffer.pipelineBarrier2(depenInfo);
    }

    static vk::CommandBuffer BeginOneTimeCommandBuffer(const vk::Device device, const vk::CommandPool commandPool){
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;

        auto tempCommandBuffer = device.allocateCommandBuffers(allocInfo).value.at(0);

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        
        tempCommandBuffer.begin(beginInfo);

        return tempCommandBuffer;
    }

    static void EndOneTimeCommandBuffer(const vk::Device device, const vk::CommandPool commandPool, const vk::CommandBuffer commandBuffer, const vk::Queue queue){
        
        commandBuffer.end();

        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlags{};
        auto submitDoneFence = device.createFence(fenceInfo).value;

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        
        auto res = queue.submit(submitInfo, submitDoneFence);

        if(res != vk::Result::eSuccess){
            std::println("Failed Submit Data");
        }

        res = device.waitForFences(submitDoneFence, vk::True, std::numeric_limits<uint64_t>::max());

        if(res != vk::Result::eSuccess){
            std::println("Failed Wait Submit Data");
        }

        device.destroyFence(submitDoneFence);
        device.freeCommandBuffers(commandPool, commandBuffer);
    }
}