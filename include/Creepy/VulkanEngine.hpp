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
#include "Model.hpp"
#include "Camera.hpp"
#include "Material.hpp"

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
            void drawModels(const vk::CommandBuffer currentCommandBuffer, const vk::ImageView colorImageView, const vk::ImageView depthImageView);
            void drawImGui(const vk::CommandBuffer currentCommandBuffer, const vk::ImageView colorImageView);
            void drawSkyBox(const vk::CommandBuffer currentCommandBuffer, const vk::ImageView colorImageView);

            uint32_t getEntityAtPixel(uint32_t x, uint32_t y);

        private:
            int m_windowWidth{}, m_windowHeight{};
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

            Buffer<BufferType::HOST_COHERENT> m_objectsPickingBuffer;
            Buffer<BufferType::HOST_COHERENT> m_screenShotBuffer;

            size_t m_totalFrames{}, m_currentFrame{};
            std::vector<VulkanFrame> m_renderFrames;

            vk::DescriptorPool m_descriptorPool;
            DescriptorSet m_uniformBufferDescriptorSet;
            DescriptorSet m_descriptorIndexingDescriptorSet;
            DescriptorSet m_skyBoxDescriptorSet;

            // Resources
            Image m_entityImage;
            Image m_depthImage;

            // Pipeline
            std::unordered_map<std::string, Pipeline> m_pipelines;

            TextureCubeMap m_skyBoxTexture{};

            vma::Allocator m_allocator;
            VulkanJobSystem m_clearner;
            
            // TODO: Impl
            // VulkanCommandBufferSubmitData m_submitter;

            UniformBuffer m_uniformBuffer;
            TransformData m_transformData;
            LightData m_lightData;

            std::unordered_map<std::string, Model> m_models;
            std::unordered_map<std::string, Model> m_primitiveModels;
            
            Camera m_camera{};
            //TODO: Use atomic
            bool m_isSwapchainResizing{false};
            bool m_isEnableMousePicking{false};
            bool m_isEnableScreenShot{false};
    };
}