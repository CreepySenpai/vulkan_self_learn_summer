#include <print>
#include <Creepy/Debug.hpp>
#include <Creepy/Uniform.hpp>
#include <Creepy/Material.hpp>

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

    void Debug::DrawLightData(LightData& lightData) {
        ImGui::Begin("Light");

        ImGui::DragFloat3("Position", glm::value_ptr(lightData.lightPosition));
        ImGui::DragFloat3("Ambient", glm::value_ptr(lightData.ambientColor), 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat3("Diffuse", glm::value_ptr(lightData.diffuseIntensity), 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat3("Intensity", glm::value_ptr(lightData.lightIntensity), 0.1f, 0.0f, 1.0f);

        ImGui::End();
    }

    void Debug::DrawModelInfo(std::unordered_map<std::string, Model>& models, MaterialManager& materialManager) {
        ImGui::Begin("Models");

        for(auto& [modelName, model] : models){
            ImGui::Separator();
            ImGui::PushID(modelName.c_str());

            ImGui::Text("Name: %s", modelName.c_str());
            ImGui::DragFloat3("Position", glm::value_ptr(model.GetPosition()), 0.2f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(model.GetRotation()), 0.2f);
            ImGui::DragFloat3("Scale", glm::value_ptr(model.GetScale()), 0.2f);
            auto& materialInfo =  materialManager.GetMaterialData(model.GetMaterialIndex());
            ImGui::DragFloat3("Material Ambient", glm::value_ptr(materialInfo.materialAmbient), 0.1f, 0.0f, 1.0f);
            ImGui::DragFloat3("Material Diffuse", glm::value_ptr(materialInfo.materialDiffuse), 0.1f, 0.0f, 1.0f);
            ImGui::DragFloat3("Material Specular", glm::value_ptr(materialInfo.materialSpecular), 0.1f, 0.0f, 1.0f);
            
            ImGui::PopID();
        }

        ImGui::End();
    }

    void Debug::EndFrame() {
        ImGui::Render();
    }

}