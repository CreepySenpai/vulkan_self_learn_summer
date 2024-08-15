#pragma once

#include <filesystem>
#include "Mesh.hpp"

struct aiScene;
struct aiNode;
struct aiMesh;

namespace Creepy{

    class Model{
        public:
            Model() = default;
            Model(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            
            void Destroy(const vk::Device device) const;

        private:
            void loadModel(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            void processNode(aiNode* node, const aiScene* scene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);
            Mesh processMesh(aiMesh* mesh, const aiScene* scene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue);

        private:
            std::vector<Mesh> m_meshes;
    };
    
}