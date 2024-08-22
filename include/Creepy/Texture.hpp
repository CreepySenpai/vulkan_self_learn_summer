#pragma once

#include <filesystem>
#include "VulkanImage.hpp"


namespace Creepy{

    class Texture{
        public:
            void LoadTexture(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            
            vk::Image GetImage() const;
            vk::ImageView GetImageView() const;
            vk::Sampler GetSampler() const;
            
            vk::DescriptorSet GetDescriptorSet() const;
            void SetDescriptorSet(const vk::DescriptorSet descriptorSet);

            void Destroy(const vk::Device device) const;

        private:
            void createSampler(const vk::Device device);
        private:
            Image m_image;
            vk::Sampler m_sampler;
            vk::DescriptorSet m_imageDescriptorSet;
    };

}