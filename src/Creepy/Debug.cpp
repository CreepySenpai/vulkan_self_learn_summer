#include <Creepy/Debug.hpp>
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

    void Debug::EndFrame() {
        ImGui::Render();
    }

}