#include <print>
#include <Creepy/VulkanImage.hpp>
#include <Creepy/VulkanAllocator.hpp>

namespace Creepy {

    Image::Image(const vk::Device device, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags imageUsage, vk::ImageAspectFlags aspect)
        : m_width{width}, m_height{height}, m_imageFormat{format}
    {
        std::println("Call Create Image");
        this->createImage(device, imageUsage, aspect);
        this->createImageView(device, aspect);
    }

    void Image::Destroy(const vk::Device device) const {
        device.destroyImageView(m_imageView);
        VulkanAllocator::ImageAllocator.destroyImage(m_image, m_imageLoc);
    }

    vk::Image Image::GetImage() const {
        return m_image;
    }

    vk::ImageView Image::GetImageView() const {
        return m_imageView;
    }

    vk::Format Image::GetImageFormat() const {
        return m_imageFormat;
    }

    vk::Extent2D Image::GetImageExtent() const {
        return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)};
    }

    void Image::ReCreate(const vk::Device device, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags imageUsage, vk::ImageAspectFlags aspect) {
        std::println("Call Re Image: {} - {}", width, height);
        m_width = width;
        m_height = height;
        m_imageFormat = format;
        this->Destroy(device);
        this->createImage(device, imageUsage, aspect);
        this->createImageView(device, aspect);
        std::println("Image Size: {} - {}", m_width, m_height);
    }

    void Image::createImage(const vk::Device device, vk::ImageUsageFlags imageUsage, vk::ImageAspectFlags aspect) {
        vk::ImageCreateInfo info{};
        info.flags = vk::ImageCreateFlags{};
        info.format = m_imageFormat;
        info.imageType = vk::ImageType::e2D;
        info.initialLayout = vk::ImageLayout::eUndefined;
        info.usage = imageUsage;
        info.tiling = vk::ImageTiling::eOptimal;

        info.extent.width = m_width;
        info.extent.height = m_height;
        info.extent.depth = 1;
        
        info.arrayLayers = 1;
        info.mipLevels = 1;
        info.sharingMode = vk::SharingMode::eExclusive;
        info.samples = vk::SampleCountFlagBits::e1;
        

        vma::AllocationCreateInfo allocInfo{};
        allocInfo.flags = vma::AllocationCreateFlags{};
        allocInfo.usage = vma::MemoryUsage::eGpuOnly;
        allocInfo.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        
        auto res = VulkanAllocator::ImageAllocator.createImage(info, allocInfo);
        
        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Image");
        }

        std::tie(m_image, m_imageLoc) = res.value;
    }

    void Image::createImageView(const vk::Device device, vk::ImageAspectFlags aspect) {
         vk::ImageViewCreateInfo imageViewInfo{};
        imageViewInfo.flags = vk::ImageViewCreateFlags{};
        imageViewInfo.format = m_imageFormat;
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
    

    void Image::CopyImage(const vk::CommandBuffer commandBuffer, const vk::Image srcImage, const vk::Image dstImage, const vk::Extent2D srcSize, const vk::Extent2D dstSize) {
        vk::ImageBlit2 blitRegion{};
        blitRegion.srcOffsets.at(1).x = srcSize.width;
        blitRegion.srcOffsets.at(1).y = srcSize.height;
        blitRegion.srcOffsets.at(1).z = 1;

        blitRegion.dstOffsets.at(1).x = dstSize.width;
        blitRegion.dstOffsets.at(1).y = dstSize.height;
        blitRegion.dstOffsets.at(1).z = 1;

        // TODO: Handle More Image Type
        blitRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blitRegion.srcSubresource.baseArrayLayer = 0;
        blitRegion.srcSubresource.mipLevel = 0;
        blitRegion.srcSubresource.layerCount = 1;

        blitRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blitRegion.dstSubresource.baseArrayLayer = 0;
        blitRegion.dstSubresource.mipLevel = 0;
        blitRegion.dstSubresource.layerCount = 1;
        
        vk::BlitImageInfo2 blitInfo{};
        blitInfo.srcImage = srcImage;
        blitInfo.srcImageLayout = vk::ImageLayout::eTransferSrcOptimal;
        blitInfo.dstImage = dstImage;
        blitInfo.dstImageLayout = vk::ImageLayout::eTransferDstOptimal;
        blitInfo.filter = vk::Filter::eLinear;
        blitInfo.regionCount = 1;
        blitInfo.pRegions = &blitRegion;

        commandBuffer.blitImage2(blitInfo);
    }
}