#pragma once

#include <unordered_map>

#include <vulkan/vulkan.hpp>
#include <vkmemoryalloc/vk_mem_alloc.hpp>
#include "VulkanJobSystem.hpp"
#include "VulkanImage.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanFrame.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanDescriptor.hpp"
#include "Texture.hpp"
#include "Model.hpp"
#include "Camera.hpp"

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

            void createDescriptorPool();

            void initImGUI();

            void createDescriptorSets();
            void createPipelines();

            void recreateSwapchain();
        private:
            void createResources();
            void createImageResources();
            void createBufferResources();
            void loadModels();
            
            const VulkanFrame& getCurrentRenderFrame() const;

            void updateUniformBuffer();

        private:
            void createCamera();
            void onUpdate(double deltaTime);

        private:
            void onDraw();
            void drawModels(const vk::CommandBuffer currentCommandBuffer, const vk::Image colorImage, const vk::ImageView colorImageView, const vk::Image depthImage, const vk::ImageView depthImageView);
            void drawImGui(const vk::CommandBuffer currentCommandBuffer, const vk::Image colorImage, const vk::ImageView colorImageView);
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

            Swapchain m_swapchain;
            //TODO: List of cmdBuffer

            size_t m_totalFrames{}, m_currentFrame{};
            std::vector<VulkanFrame> m_renderFrames;

            vk::DescriptorPool m_descriptorPool;
            DescriptorSet m_uniformBufferDescriptorSet;
            vk::DescriptorSetLayout m_textureDescriptorSetLayout;

            // Resources
            // Image m_colorImage;
            Image m_depthImage;

            // VertexBuffer m_triangleVertexBuffer;
            // IndexBuffer m_triangleIndexBuffer;

            // Pipeline
            Pipeline m_backgroundPipeline{};

            vma::Allocator m_allocator;
            VulkanJobSystem m_clearner;
            VulkanCommandBufferSubmitData m_submitter;

            UniformBuffer m_uniformBuffer;
            UniformData m_uniformData;

            std::unordered_map<std::string, Model> m_models;

            Camera m_camera{};
            //TODO: Use atomic
            bool m_isSwapchainResizing{false};
    };
}