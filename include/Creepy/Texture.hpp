#pragma once

#include <unordered_map>
#include <variant>
#include <filesystem>
#include "VulkanImage.hpp"


namespace Creepy{

    class BaseTexture {
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

            bool IsUpdateDescriptorSet() const {
                return m_isUpdateDescriptorSet;
            }

            void UpdateDescriptorSet(){
                m_isUpdateDescriptorSet = true;
            }

            void Destroy(const vk::Device device) const {
                device.destroySampler(m_sampler); 
                m_image.Destroy(device);
            }

        protected:
            Image m_image;
            vk::Sampler m_sampler;
            vk::DescriptorSet m_imageDescriptorSet;
            bool m_isUpdateDescriptorSet{false};
    };

    class Texture : public BaseTexture {
        public:
            void LoadTexture(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
        private:
            void createSampler(const vk::Device device);
    };

    class TextureCubeMap : public BaseTexture {
        public:
            void LoadTextureCubeMap(std::span<const std::filesystem::path> filePaths, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
        private:
            void createSampler(const vk::Device device);
    };

    class TextureManager {
        public:
            using TextureType = std::variant<Texture, TextureCubeMap>;
            
            static void LoadTexture2D(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            static void LoadTextureCubeMap(const std::string& textureName, std::span<const std::filesystem::path> filePaths, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            static bool IsContainTexture(const std::string& textureName);
            static bool IsContainTexture(const std::filesystem::path& texturePath);

            static void Destroy(const vk::Device device);

            template <typename T>
            requires std::derived_from<T, BaseTexture>
            [[nodiscard]] static inline T* GetTexture(const std::string& textureName){
                auto&& textureAtName = s_texturesMap.at(textureName);
                if constexpr(std::same_as<T, Texture>){
                    return std::get_if<Texture>(&textureAtName);
                }
                else if constexpr(std::same_as<T, TextureCubeMap>){
                    return std::get_if<TextureCubeMap>(&textureAtName);
                }
                else{
                    static_assert(false, "Wrong Texture Type");
                    return nullptr;
                }
            }

            template <typename T>
            requires std::derived_from<T, BaseTexture>
            [[nodiscard]] static inline T* GetTexture(const std::filesystem::path& texturePath){
                auto&& textureName = texturePath.stem().string();
                return TextureManager::GetTexture<T>(textureName);
            }

        private:
            static inline std::unordered_map<std::string, TextureType> s_texturesMap;
    };
}