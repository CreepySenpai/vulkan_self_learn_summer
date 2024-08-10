#pragma once

#include <vulkan/vulkan.hpp>
#include <vkmemoryalloc/vk_mem_alloc.hpp>
#include "VulkanJobSystem.hpp"
#include "VulkanImage.hpp"
#include "VulkanPipeline.hpp"

struct GLFWwindow;

namespace Creepy {

    class VulkanEngine
    {
        public:
            VulkanEngine();
            ~VulkanEngine();

            void Run();
        private:
            void createWindow();
            void createInstance();
            void createDebugMessage();
            void createSurface();
            void createDevice();
            void createAllocator();
            void createQueue();
            void createCommandPool();
            void createSwapchain();
            void createCommandBuffer();
            void createSync();

            void createDecriptorPool();

            void initImGUI();

            void createDescriptorSets();
            void createPipelines();
        private:
            void createImageResources();
            void draw();

        private:
            int m_width{}, m_height{};
            GLFWwindow* m_window{nullptr};
            vk::Instance m_instance;
            vk::DispatchLoaderDynamic m_dispatcher;
            vk::DebugUtilsMessengerEXT m_debugUtils;
            vk::PhysicalDevice m_physicalDevice;
            vk::Device m_logicalDevice;
            vk::SurfaceKHR m_surface;
            vk::Queue m_graphicQueue;
            vk::Queue m_presentQueue;
            vk::CommandPool m_cmdPool;
            vk::SwapchainKHR m_swapChain;
            std::vector<vk::Image> m_swapchainImages;
            vk::Format m_swapchainImageFormat;
            //TODO: List of cmdBuffer
            vk::CommandBuffer m_commandBuffer;
            vk::Semaphore m_acquireImageSemaphore, m_renderStartSemaphore;
            vk::Fence m_renderCompleteFence;

            vk::DescriptorPool m_descriptorPool;
            // Resources

            Image m_colorImage, m_depthImage;

            // Pipeline
            Pipeline m_backgroundPipeline{};

            vma::Allocator m_allocator;
            VulkanJobSystem m_clearner;
    };
}