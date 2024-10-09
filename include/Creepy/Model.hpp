#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "Mesh.hpp"

namespace Creepy{

    class Model{
        public:
            Model() = default;
            Model(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            
            void Destroy(const vk::Device device) const;

            std::span<const Mesh> GetMeshes() const;
            std::span<Mesh> GetMeshes();

            void LoadModel(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);

            void Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets);
            
            void Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets, FragmentPushConstantData fragmentPushConstantData);
            
            void Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, FragmentPushConstantData fragmentPushConstantData);

            glm::vec3& GetPosition();

            glm::vec3& GetRotation();

            glm::vec3& GetScale();

            void SetMaterialIndex(uint32_t materialIndex);
            uint32_t GetMaterialIndex() const;
        private:
            glm::mat4 getTransformMatrix() const;

        private:
            void processNode(aiNode* currentNode, const aiScene* currentScene, const glm::mat4& parentTransformMatrix, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            Mesh processMeshInterLeaved(aiMesh* currentMesh, const aiScene* currentScene, const glm::mat4& parentTransformMatrix, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            Mesh processMeshSeparate(aiMesh* currentMesh, const aiScene* currentScene, const glm::mat4& parentTransformMatrix, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);

            std::vector<Texture*> loadMaterialTextures(aiMaterial* currentMaterial, const aiScene* currentScene, aiTextureType textureType, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
        private:
            std::vector<Mesh> m_meshes;
            glm::vec3 m_position{};
            glm::vec3 m_rotation{};
            glm::vec3 m_scale{1.0f, 1.0f, 1.0f};
            uint32_t m_materialIndex{};
            uint32_t m_entityID{};
    };
    
}