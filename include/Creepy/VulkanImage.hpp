#pragma once

#include <vulkan/vulkan.hpp>
#include <vkmemoryalloc/vk_mem_alloc.hpp>

namespace Creepy {
    class Image
    {
        public:
            Image() = default;
            Image(const vk::Device device, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags imageUsage, vk::ImageAspectFlags aspect);

            void Destroy(const vk::Device device) const;

            vk::Image GetImage() const;

            vk::ImageView GetImageView() const;
            
            vk::Format GetImageFormat() const;

            vk::Extent2D GetImageExtent() const;

            void ReCreate(const vk::Device device, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags imageUsage, vk::ImageAspectFlags aspect);
            
            void CreateImageView(const vk::Device device, vk::ImageAspectFlags aspect);
        public:
            static void CopyImage(const vk::CommandBuffer commandBuffer, const vk::Image srcImage, const vk::Image dstImage, const vk::Extent2D srcSize, const vk::Extent2D dstSize);
            
        private:
            void createImage(const vk::Device device, vk::ImageUsageFlags imageUsage, vk::ImageAspectFlags aspect);

        private:
            vk::Image m_image;
            vk::ImageView m_imageView;
            vma::Allocation m_imageLoc;
            uint32_t m_width{}, m_height{};
            vk::Format m_imageFormat;
    };
}