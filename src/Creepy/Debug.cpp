#include <Creepy/Debug.hpp>
#include <Creepy/Uniform.hpp>
#include <imgui/imgui.hpp>
#include <imgui/imgui_impl_vulkan.hpp>
#include <imgui/imgui_impl_glfw.hpp>

namespace Creepy{

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