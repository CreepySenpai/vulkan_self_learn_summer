#include <Creepy/Texture.hpp>
#include <Creepy/VulkanUtils.hpp>
#include <Creepy/VulkanBuffer.hpp>
#include <stb/stb_image.h>

namespace Creepy{

    void Texture::LoadTexture(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue) {
        int width{}, height{}, channel{};
        
        auto textureData = stbi_load(filePath.string().c_str(), &width, &height, &channel, 4);

        m_image = Image{device, static_cast<uint32_t>(width), static_cast<uint32_t>(height), vk::Format::eR8G8B8A8Unorm, 
            vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::ImageAspectFlagBits::eColor};

        Buffer<BufferType::HOST_VISIBLE> stagingBuffer{device, 
            static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * sizeof(uint32_t), 
            vk::BufferUsageFlagBits::eTransferSrc};

        stagingBuffer.UploadData(textureData, static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * sizeof(uint32_t));

        auto tempCommandBuffer = BeginOneTimeCommandBuffer(device, commandPool);

        ////////////////////////////////////////////////////////////////////////////
        
        imageLayoutTransition(tempCommandBuffer, m_image.GetImage(), vk::ImageAspectFlagBits::eColor, 
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 
            vk::AccessFlagBits2::eMemoryRead, vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,
             vk::PipelineStageFlagBits2::eAllCommands, vk::PipelineStageFlagBits2::eAllCommands);

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
            vk::AccessFlagBits2::eMemoryRead, vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,
             vk::PipelineStageFlagBits2::eAllCommands, vk::PipelineStageFlagBits2::eAllCommands);


        ///////////////////////////////////////////////////////////////////////////
        
        EndOneTimeCommandBuffer(device, commandPool, tempCommandBuffer, queue);

        stagingBuffer.Destroy(device);

        stbi_image_free(textureData);
    }

    void Texture::Destroy(const vk::Device device) const {
        m_image.Destroy(device);
    }

}