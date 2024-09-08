#include <Creepy/Texture.hpp>
#include <Creepy/VulkanUtils.hpp>
#include <Creepy/VulkanBuffer.hpp>
#include <stb/stb_image.h>

namespace Creepy{

    void Texture::LoadTexture(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue) {
        int width{}, height{}, channel{};
        
        auto textureData = stbi_load(filePath.string().c_str(), &width, &height, &channel, STBI_rgb_alpha);
        
        std::println("Load Texture: {} - {} - {}",filePath.string(), width, height);

        m_image = Image{device, static_cast<uint32_t>(width), static_cast<uint32_t>(height), vk::Format::eR8G8B8A8Unorm, 
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlagBits::eColor, vk::ImageViewType::e2D};

        this->createSampler(device);
        
        const uint64_t bufferSize{static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * sizeof(uint32_t)};
        Buffer<BufferType::HOST_VISIBLE> stagingBuffer{device, 
            bufferSize, 
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
        copyRegion.imageExtent = vk::Extent3D{static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
        copyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageOffset.x = 0;
        copyRegion.imageOffset.y = 0;
        copyRegion.imageOffset.z = 0;
        
        tempCommandBuffer.copyBufferToImage(stagingBuffer.GetBuffer(), m_image.GetImage(), vk::ImageLayout::eTransferDstOptimal, copyRegion);

        imageLayoutTransition(tempCommandBuffer, m_image.GetImage(), vk::ImageAspectFlagBits::eColor, 
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 
            vk::AccessFlagBits2::eTransferWrite, vk::AccessFlagBits2::eShaderRead,
             vk::PipelineStageFlagBits2::eTransfer, vk::PipelineStageFlagBits2::eFragmentShader);


        ///////////////////////////////////////////////////////////////////////////
        
        EndOneTimeCommandBuffer(device, commandPool, tempCommandBuffer, queue);

        stagingBuffer.Destroy(device);

        stbi_image_free(textureData);
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

    vk::DescriptorSet Texture::GetDescriptorSet() const {
        return m_imageDescriptorSet;
    }

    void Texture::SetDescriptorSet(const vk::DescriptorSet descriptorSet) {
        m_imageDescriptorSet = descriptorSet;
    }

    void Texture::Destroy(const vk::Device device) const {
        device.destroySampler(m_sampler);
        m_image.Destroy(device);
    }

    void Texture::createSampler(const vk::Device device) {
        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.flags = vk::SamplerCreateFlags{};
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
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

    ////////////////////////////////////////////////////////////////

    void TextureCubeMap::LoadCubeMapTexture(std::span<const std::filesystem::path> cubeMapPaths, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue) {
        int width{}, height{}, channel{};

        std::array<stbi_uc*, 6> cubeMapTextureData;
        for(size_t i{}; auto&& filePath : cubeMapPaths){
            cubeMapTextureData.at(i) = stbi_load(filePath.string().c_str(), &width, &height, &channel, STBI_rgb_alpha);
            ++i;
        }

        m_image = Image{device, static_cast<uint32_t>(width), static_cast<uint32_t>(height), vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlagBits::eColor, vk::ImageViewType::eCube};

        this->createSampler(device);

        const uint64_t bufferSize{static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * sizeof(uint32_t) * 6u};   // Size * RGBA * 6 Face
        const uint64_t perImageSize{bufferSize / 6};

        Buffer<BufferType::HOST_VISIBLE> stagingBuffer{device, 
            bufferSize,
            vk::BufferUsageFlagBits::eTransferSrc};
        
        for(uint32_t i{}; auto data : cubeMapTextureData){
            stagingBuffer.UploadData(data, perImageSize, i * perImageSize);
            ++i;
        }

        auto tempCommandBuffer = BeginOneTimeCommandBuffer(device, commandPool);

        //////////////////////////////////////////

        imageLayoutTransition(tempCommandBuffer, m_image.GetImage(), vk::ImageAspectFlagBits::eColor, 
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 
            vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eTransferWrite,
             vk::PipelineStageFlagBits2::eHost, vk::PipelineStageFlagBits2::eTransfer, 6);

        std::array<vk::BufferImageCopy, 6> copyRegions{};

        for(uint32_t i{}; auto& copyRegion : copyRegions){

            copyRegion.bufferOffset = i * perImageSize;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;
            copyRegion.imageExtent = vk::Extent3D{static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
            copyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            copyRegion.imageSubresource.baseArrayLayer = i; // Current Face
            copyRegion.imageSubresource.mipLevel = 0;   
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageOffset.x = 0;
            copyRegion.imageOffset.y = 0;
            copyRegion.imageOffset.z = 0;

            ++i;
        }
        
        tempCommandBuffer.copyBufferToImage(stagingBuffer.GetBuffer(), m_image.GetImage(), vk::ImageLayout::eTransferDstOptimal, copyRegions);

        imageLayoutTransition(tempCommandBuffer, m_image.GetImage(), vk::ImageAspectFlagBits::eColor, 
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 
            vk::AccessFlagBits2::eTransferWrite, vk::AccessFlagBits2::eShaderRead,
             vk::PipelineStageFlagBits2::eTransfer, vk::PipelineStageFlagBits2::eFragmentShader, 6);

        //////////////////////////////////////////
        EndOneTimeCommandBuffer(device, commandPool, tempCommandBuffer, queue);
        
        stagingBuffer.Destroy(device);

        for(auto data : cubeMapTextureData){
            stbi_image_free(data);
        }
    }

    vk::Image TextureCubeMap::GetImage() const {
        return m_image.GetImage();
    }

    vk::ImageView TextureCubeMap::GetImageView() const {
        return m_image.GetImageView();
    }

    vk::Sampler TextureCubeMap::GetSampler() const {
        return m_sampler;
    }
            
    vk::DescriptorSet TextureCubeMap::GetDescriptorSet() const {
        return m_imageDescriptorSet;
    }

    void TextureCubeMap::SetDescriptorSet(const vk::DescriptorSet descriptorSet) {

    }

    void TextureCubeMap::Destroy(const vk::Device device) const {
        device.destroySampler(m_sampler);
        m_image.Destroy(device);
    }

    void TextureCubeMap::createSampler(const vk::Device device) {
        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.flags = vk::SamplerCreateFlags{};
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
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