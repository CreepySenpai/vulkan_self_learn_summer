#include <Creepy/Texture.hpp>
#include <Creepy/VulkanUtils.hpp>
#include <Creepy/VulkanBuffer.hpp>
#include <stb/stb_image.h>

namespace Creepy{

    void Texture::LoadTexture(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue) {
        int width{}, height{}, channel{};
        
        auto textureData = stbi_load(filePath.string().c_str(), &width, &height, &channel, 4);

        m_image = Image{device, static_cast<uint32_t>(width), static_cast<uint32_t>(height), vk::Format::eR8G8B8A8Unorm, 
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlagBits::eColor};

        Buffer<BufferType::HOST_VISIBLE> stagingBuffer{device, 
            static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * sizeof(uint32_t), 
            vk::BufferUsageFlagBits::eTransferSrc};

        stagingBuffer.UploadData(textureData, static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * sizeof(uint32_t));

        auto tempCommandBuffer = BeginOneTimeCommandBuffer(device, commandPool);

        ////////////////////////////////////////////////////////////////////////////
        
        imageLayoutTransition(tempCommandBuffer, m_image.GetImage(), vk::ImageAspectFlagBits::eColor, 
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 
            vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eTransferWrite,
             vk::PipelineStageFlagBits2::eHost, vk::PipelineStageFlagBits2::eTransfer);

        vk::BufferImageCopy copyRegion{};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        copyRegion.imageExtent = vk::Extent3D{m_image.GetImageExtent(), 1};
        copyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.layerCount = 1;
        
        tempCommandBuffer.copyBufferToImage(stagingBuffer.GetBuffer(), m_image.GetImage(), vk::ImageLayout::eTransferDstOptimal, copyRegion);

        imageLayoutTransition(tempCommandBuffer, m_image.GetImage(), vk::ImageAspectFlagBits::eColor, 
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 
            vk::AccessFlagBits2::eTransferWrite, vk::AccessFlagBits2::eShaderRead,
             vk::PipelineStageFlagBits2::eTransfer, vk::PipelineStageFlagBits2::eFragmentShader);


        ///////////////////////////////////////////////////////////////////////////
        
        EndOneTimeCommandBuffer(device, commandPool, tempCommandBuffer, queue);

        stagingBuffer.Destroy(device);

        stbi_image_free(textureData);

        this->createSampler(device);
    }
    
    vk::Image Texture::GetImage() const {
        return m_image.GetImage();
    }

    vk::ImageView Texture::GetImageView() const {
        return m_image.GetImageView();
    }

    vk::Sampler Texture::GetSampler() const {
        return m_sampler;
    }

    void Texture::Destroy(const vk::Device device) const {
        device.destroySampler(m_sampler);
        m_image.Destroy(device);
    }

    void Texture::createSampler(const vk::Device device) {
        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.flags = vk::SamplerCreateFlags{};
        samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
        samplerInfo.minFilter = vk::Filter::eNearest;
        samplerInfo.magFilter = vk::Filter::eNearest;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.minLod = 0.0f;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.compareOp = vk::CompareOp::eNever;
        samplerInfo.maxLod = 0.0f;
        samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;
        samplerInfo.anisotropyEnable = vk::False;
        samplerInfo.maxAnisotropy = 1.0f;

        m_sampler = device.createSampler(samplerInfo).value;
    }

}