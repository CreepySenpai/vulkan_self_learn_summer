#pragma once

#include <filesystem>
#include "Mesh.hpp"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

namespace Creepy{

    class Model{
        public:
            Model() = default;
            Model(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            
            void Destroy(const vk::Device device) const;

            std::span<const Mesh> GetMeshes() const;
            std::span<Mesh> GetMeshes();

            void LoadModel(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
        
        private:
            void processNode(aiNode* currentNode, const aiScene* currentScene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            Mesh processMesh(aiMesh* currentMesh, const aiScene* currentScene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);

            std::vector<Texture> loadMaterialTextures(aiMaterial* currentMaterial, const aiScene* currentScene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
        private:
            std::vector<Mesh> m_meshes;
    };
    
}