#pragma once

#include <vulkan/vulkan.hpp>

struct VulkanFrame{
    vk::CommandBuffer m_commandBuffer;
    vk::Fence m_renderCompleteFence;
    vk::Semaphore m_imageAvailableSemaphore, m_imageRenderedSemaphore;
};