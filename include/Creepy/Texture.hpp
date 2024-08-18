#pragma once

#include <filesystem>
#include "VulkanImage.hpp"


namespace Creepy{

    class Texture{
        public:
            void LoadTexture(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);

            void Destroy(const vk::Device device) const;
        private:
            Image m_image;
    };

}