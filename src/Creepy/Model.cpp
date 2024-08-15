#include <Creepy/Model.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Creepy{

    Model::Model(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue) {
        this->loadModel(filePath, device, commandPool, queue);
    }
            
    void Model::Destroy(const vk::Device device) const {
        for(auto&& mesh : m_meshes){
            mesh.Destroy(device);
        }
    }

    void Model::loadModel(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){
        Assimp::Importer importer{};

        auto scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

        if(!scene){
            std::println("Failed to load model: {}", filePath.string());
        }
        
        this->processNode(scene->mRootNode, scene, device, commandPool, queue);

        std::println("Total Mesh: {}", m_meshes.size());
    }

    void Model::processNode(aiNode* node, const aiScene* scene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){

        for(uint32_t i{}; i < node->mNumMeshes; ++i){
            auto currentMesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(this->processMesh(currentMesh, scene, device, commandPool, queue));
        }

        // Child Node
        for(uint32_t i{}; i < node->mNumChildren; ++i){
            processNode(node->mChildren[i], scene, device, commandPool, queue);
        }
    }

    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){
        std::vector<Vertex> vertices(mesh->mNumVertices);

        for(uint32_t i{}; i < mesh->mNumVertices; ++i){
            vertices[i].Position.x = mesh->mVertices[i].x;
            vertices[i].Position.y = mesh->mVertices[i].y;
            vertices[i].Position.z = mesh->mVertices[i].z;


            vertices[i].Normal.x = mesh->mNormals[i].x;
            vertices[i].Normal.y = mesh->mNormals[i].y;
            vertices[i].Normal.z = mesh->mNormals[i].z;
            
            
            if(mesh->mTextureCoords[0]){
                vertices[i].TexCoord.x = mesh->mTextureCoords[0][i].x;
                vertices[i].TexCoord.y = mesh->mTextureCoords[0][i].y;
            }
        }

        std::vector<uint32_t> indices;

        for(uint32_t i{}; i < mesh->mNumFaces; ++i){
            
            auto currentFace = mesh->mFaces[i];

            for(uint32_t j{}; j < currentFace.mNumIndices; ++j){
                indices.emplace_back(currentFace.mIndices[j]);
            }

        }

        return {device, commandPool, queue, vertices, indices};
    }

}