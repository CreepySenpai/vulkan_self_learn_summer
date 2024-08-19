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

            void Destroy(const vk::Device device) const;

        private:
            void createSampler(const vk::Device device);
        private:
            Image m_image;
            vk::Sampler m_sampler{};
    };

}