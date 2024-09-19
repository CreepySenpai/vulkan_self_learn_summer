#pragma once

#include <filesystem>
#include "VulkanImage.hpp"


namespace Creepy{

    class BaseTexture{
        public:
            vk::Image GetImage() const {
                return m_image.GetImage();
            }

            vk::ImageView GetImageView() const {
                return m_image.GetImageView();
            }

            vk::Sampler GetSampler() const {
                return m_sampler;
            }

            vk::DescriptorSet GetDescriptorSet() const {
                return m_imageDescriptorSet;
            }

            void SetDescriptorSet(const vk::DescriptorSet descriptorSet) {
                m_imageDescriptorSet = descriptorSet;
            }

            void Destroy(const vk::Device device) const {
                device.destroySampler(m_sampler); 
                m_image.Destroy(device);
            }

        protected:
            Image m_image;
            vk::Sampler m_sampler;
            vk::DescriptorSet m_imageDescriptorSet;
    };

    class Texture : public BaseTexture {
        public:
            void LoadTexture(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
        private:
            void createSampler(const vk::Device device);
    };

    class TextureCubeMap : public BaseTexture{
        public:
            void LoadCubeMapTexture(std::span<const std::filesystem::path> cubeMapPaths, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
        private:
            void createSampler(const vk::Device device);
    };
}