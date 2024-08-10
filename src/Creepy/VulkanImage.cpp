#include <print>
#include <Creepy/VulkanImage.hpp>
#include <Creepy/VulkanAllocator.hpp>

namespace Creepy {

    Image::Image(const vk::Device device, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags imageUsage, vk::ImageAspectFlags aspect)
        : m_width{width}, m_height{height}, m_imageFormat{format}
    {
        vk::ImageCreateInfo info{};
        info.flags = vk::ImageCreateFlags{};
        info.imageType = vk::ImageType::e2D;
        info.extent.width = width;
        info.extent.height = height;
        info.extent.depth = 1;

        info.format = format;
        info.tiling = vk::ImageTiling::eOptimal;
        info.arrayLayers = 1;
        info.mipLevels = 1;
        info.sharingMode = vk::SharingMode::eExclusive;
        info.samples = vk::SampleCountFlagBits::e1;
        info.initialLayout = vk::ImageLayout::eUndefined;
        info.usage = imageUsage;
        info.queueFamilyIndexCount = 1;


        vma::AllocationCreateInfo allocInfo{};
        allocInfo.flags = vma::AllocationCreateFlags{};
        allocInfo.usage = vma::MemoryUsage::eGpuOnly;
        allocInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        
        std::println("Start Alloc");
        
        auto res = VulkanAllocator::ImageAllocator.createImage(info, allocInfo);
        std::println("Done Alloc");
        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Image");
        }

        std::tie(m_image, m_imageLoc) = res.value;
        
        vk::ImageViewCreateInfo imageViewInfo{};
        imageViewInfo.flags = vk::ImageViewCreateFlags{};
        imageViewInfo.format = format;
        imageViewInfo.image = m_image;
        imageViewInfo.viewType = vk::ImageViewType::e2D;
        imageViewInfo.subresourceRange.aspectMask = aspect;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.layerCount = 1;
        imageViewInfo.components.a = vk::ComponentSwizzle::eIdentity;
        imageViewInfo.components.g = vk::ComponentSwizzle::eIdentity;
        imageViewInfo.components.b = vk::ComponentSwizzle::eIdentity;
        imageViewInfo.components.r = vk::ComponentSwizzle::eIdentity;


        auto imgViewRes = device.createImageView(imageViewInfo);

        if(imgViewRes.result != vk::Result::eSuccess){
            std::println("Failed Create ImgView");
        }

        m_imageView = imgViewRes.value;
    }

    void Image::Destroy(const vk::Device device) {
        device.destroyImageView(m_imageView);
        VulkanAllocator::ImageAllocator.destroyImage(m_image, m_imageLoc);
    }
    
}