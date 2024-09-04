#include <print>
#include <Creepy/Debug.hpp>
#include <Creepy/Uniform.hpp>

#include <GLFW/glfw3.h>
#include <imgui/imgui.hpp>
#include <imgui/imgui_impl_vulkan.hpp>
#include <imgui/imgui_impl_glfw.hpp>

namespace Creepy{

    static vk::DescriptorPool s_imguiInternalDescriptorPool{nullptr};

    void Debug::Init(GLFWwindow* window, const vk::Instance instance, const vk::PhysicalDevice physicalDevice, const vk::Device logicalDevice, const vk::Queue graphicQueue, uint32_t graphicQueueIndex, uint32_t swapchainImageCount, vk::Format swapchainImageFormat) {
        ImGui::CreateContext();
        
        auto& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();
        
        // Note(Creepy): Enable Callback unless u suck
        ImGui_ImplGlfw_InitForVulkan(window, true);

        // Big Pool For ImGui
        constexpr std::array imguiDescriptorPoolSizes{
            vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment, 1000},
        };

        constexpr uint32_t imguiMaxSets{1000u};

        vk::DescriptorPoolCreateInfo imguiDescPoolInfo{};
        imguiDescPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        imguiDescPoolInfo.maxSets = imguiMaxSets;
        imguiDescPoolInfo.poolSizeCount = static_cast<uint32_t>(imguiDescriptorPoolSizes.size());
        imguiDescPoolInfo.pPoolSizes = imguiDescriptorPoolSizes.data();

        auto res = logicalDevice.createDescriptorPool(imguiDescPoolInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create ImGui Descriptor Pool");
        }

        s_imguiInternalDescriptorPool = res.value;

        ImGui_ImplVulkan_InitInfo imguiInfo{};
        imguiInfo.Instance = instance;
        imguiInfo.PhysicalDevice = physicalDevice;
        imguiInfo.Device = logicalDevice;
        imguiInfo.Queue = graphicQueue;
        //TODO: Store Queue Family Index
        imguiInfo.QueueFamily = graphicQueueIndex;
        imguiInfo.MinImageCount = swapchainImageCount;
        imguiInfo.ImageCount = swapchainImageCount;
        imguiInfo.DescriptorPool = res.value;
        imguiInfo.UseDynamicRendering = true;


        const std::array swapchainImageFormats{
            swapchainImageFormat
        };
        
        vk::PipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(swapchainImageFormats.size());
        
        renderingInfo.pColorAttachmentFormats = swapchainImageFormats.data();

        imguiInfo.PipelineRenderingCreateInfo = renderingInfo;

        imguiInfo.MSAASamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&imguiInfo);
        ImGui_ImplVulkan_CreateFontsTexture();        
    }

    void Debug::ShutDown(const vk::Device logicalDevice) {
        ImGui_ImplVulkan_Shutdown();
            
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();

        logicalDevice.destroyDescriptorPool(s_imguiInternalDescriptorPool);
        s_imguiInternalDescriptorPool = nullptr;
    }

    void Debug::BeginFrame() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Debug::DrawFrame() {
        ImGui::Begin("Anh Ne");
            ImGui::Text("Suss");
        ImGui::End();
    }

    void Debug::DrawTransformData(struct TransformData& transformData) {
        ImGui::Begin("CamSus");
            ImGui::Text("Cam Pos");
            ImGui::DragFloat3("Position", glm::value_ptr(transformData.cameraPosition), 0.1f, 0.0f, 1.0f);
        ImGui::End();
    }

    void Debug::DrawLightData(struct LightData& lightData) {

    }

    void Debug::EndFrame() {
        ImGui::Render();
    }

}