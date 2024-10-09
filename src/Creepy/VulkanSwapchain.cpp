#include <print>
#include <Creepy/VulkanSwapchain.hpp>


namespace Creepy{
    Swapchain::Swapchain(const vk::Device device, const vk::SurfaceKHR surface, vk::Format swapchainFormat, vk::ColorSpaceKHR swapchainColorSpace, vk::Extent2D swapchainImageExtent, vk::PresentModeKHR presentMode, const vk::SurfaceCapabilitiesKHR& surfaceCap)
    : m_swapchainFormat{swapchainFormat}, m_swapchainImageExtent{swapchainImageExtent}
    {
        this->createSwapchain(device, surface, swapchainColorSpace, presentMode, surfaceCap);
    }

    void Swapchain::Destroy(const vk::Device device) const {
        for(auto& imageView : m_swapchainImageViews){
            device.destroyImageView(imageView);
        }

        device.destroySwapchainKHR(m_swapchain);
    }

    vk::SwapchainKHR Swapchain::GetSwapchainHandle() const {
        return m_swapchain;
    }

    vk::Extent2D Swapchain::GetSwapchainImageExtent() const {
        return m_swapchainImageExtent;
    }

    vk::Format Swapchain::GetSwapchainImageFormat() const {
        return m_swapchainFormat;
    }

    std::span<const vk::Image> Swapchain::GetSwapchainImages() const
    {
        return m_swapchainImages;
    }

    std::span<const vk::ImageView> Swapchain::GetSwapchainImageViews() const
    {
        return m_swapchainImageViews;
    }

    void Swapchain::Recreate(const vk::Device device, const vk::SurfaceKHR surface, vk::Format swapchainFormat, vk::ColorSpaceKHR swapchainColorSpace, vk::Extent2D swapchainImageExtent, vk::PresentModeKHR presentMode, const vk::SurfaceCapabilitiesKHR& surfaceCap) {
        std::println("Call ReSwap: {} - {}", swapchainImageExtent.width, swapchainImageExtent.height);
        m_swapchainFormat = swapchainFormat;
        m_swapchainImageExtent = swapchainImageExtent;
        this->Destroy(device);
        this->createSwapchain(device, surface, swapchainColorSpace, presentMode, surfaceCap);
    }

    void Swapchain::createSwapchain(const vk::Device device, const vk::SurfaceKHR surface, vk::ColorSpaceKHR swapchainColorSpace, vk::PresentModeKHR presentMode, const vk::SurfaceCapabilitiesKHR &surfaceCap)
    {
        // Note(Creepy): If we usage swapchain image for another layout -> we need add to here
        constexpr vk::ImageUsageFlags swapchainImageUsages = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;

        vk::SwapchainCreateInfoKHR info{};
        info.flags = vk::SwapchainCreateFlagsKHR{};
        info.surface = surface;
        info.oldSwapchain = nullptr;
        info.presentMode = presentMode;
        info.imageUsage = swapchainImageUsages;
        info.imageArrayLayers = 1;
        info.imageSharingMode = vk::SharingMode::eExclusive;

        const uint32_t imageCount{std::min(surfaceCap.maxImageCount, surfaceCap.minImageCount + 1)};
        info.minImageCount = imageCount;
        info.clipped = vk::True;
        info.preTransform = surfaceCap.currentTransform;

        info.imageFormat = m_swapchainFormat;
        info.imageColorSpace = swapchainColorSpace;
        
        // Make sure image W / H in bound
        info.imageExtent.width = std::max(surfaceCap.minImageExtent.width, std::min(m_swapchainImageExtent.width, surfaceCap.maxImageExtent.width));
        info.imageExtent.height = std::max(surfaceCap.minImageExtent.height, std::min(m_swapchainImageExtent.height, surfaceCap.maxImageExtent.height));

        std::println("Min: {} - {}", info.imageExtent.width, info.imageExtent.height);

        auto res = device.createSwapchainKHR(info);

        if (res.result != vk::Result::eSuccess)
        {
            std::println("Error createSwapchainKHR");
        }

        m_swapchain = res.value;

        m_swapchainImages = device.getSwapchainImagesKHR(m_swapchain).value;

        m_swapchainImageViews.clear();


        vk::ImageViewUsageCreateInfo imageViewUsageInfo{};
        imageViewUsageInfo.usage = swapchainImageUsages;

        for(auto image : m_swapchainImages){
            vk::ImageViewCreateInfo imgVInfo{};
            imgVInfo.pNext = &imageViewUsageInfo;
            imgVInfo.flags = vk::ImageViewCreateFlags{};
            imgVInfo.viewType = vk::ImageViewType::e2D;
            imgVInfo.image = image;
            imgVInfo.format = m_swapchainFormat;
            imgVInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            imgVInfo.subresourceRange.baseArrayLayer = 0;
            imgVInfo.subresourceRange.baseMipLevel = 0;
            imgVInfo.subresourceRange.layerCount = vk::RemainingArrayLayers;
            imgVInfo.subresourceRange.levelCount = vk::RemainingMipLevels;
            imgVInfo.components.r = vk::ComponentSwizzle::eIdentity;
            imgVInfo.components.g = vk::ComponentSwizzle::eIdentity;
            imgVInfo.components.b = vk::ComponentSwizzle::eIdentity;
            imgVInfo.components.a = vk::ComponentSwizzle::eIdentity;

            m_swapchainImageViews.push_back(device.createImageView(imgVInfo).value);
        }
    }
}