#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>


namespace Creepy{
    class Swapchain{
        public:
            Swapchain() = default;
            Swapchain(const vk::Device device, const vk::SurfaceKHR surface, vk::Format swapchainFormat, vk::ColorSpaceKHR swapchainColorSpace, vk::Extent2D swapchainImageExtent, vk::PresentModeKHR presentMode, const vk::SurfaceCapabilitiesKHR& surfaceCap);

            void Destroy(const vk::Device device) const;

            vk::SwapchainKHR GetSwapchainHandle() const;
            vk::Extent2D GetSwapchainImageExtent() const;
            vk::Format GetSwapchainImageFormat() const;

            std::span<const vk::Image> GetSwapchainImages() const;
            std::span<const vk::ImageView> GetSwapchainImageViews() const;

            void Recreate(const vk::Device device, const vk::SurfaceKHR surface, vk::Format swapchainFormat, vk::ColorSpaceKHR swapchainColorSpace, vk::Extent2D swapchainImageExtent, vk::PresentModeKHR presentMode, const vk::SurfaceCapabilitiesKHR& surfaceCap);
        private:
            void createSwapchain(const vk::Device device, const vk::SurfaceKHR surface, vk::ColorSpaceKHR swapchainColorSpace, vk::PresentModeKHR presentMode, const vk::SurfaceCapabilitiesKHR& surfaceCap);

        private:

            vk::SwapchainKHR m_swapchain;
            vk::Format m_swapchainFormat;
            vk::Extent2D m_swapchainImageExtent;
            std::vector<vk::Image> m_swapchainImages;
            std::vector<vk::ImageView> m_swapchainImageViews;
    };
}
