#pragma once

#include <vulkan/vulkan.hpp>
#include <vkmemoryalloc/vk_mem_alloc.hpp>

namespace Creepy {
    class Image
    {
        public:
            Image() = default;
            Image(const vk::Device device, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags imageUsage, vk::ImageAspectFlags aspect);

            void Destroy(const vk::Device device);
        private:
            vk::Image m_image;
            vk::ImageView m_imageView;
            vma::Allocation m_imageLoc;
            uint32_t m_width{}, m_height{};
            vk::Format m_imageFormat;
    };
}