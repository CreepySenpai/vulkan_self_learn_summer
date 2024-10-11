#include <Creepy/Model.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

// TODO: Remove Hard Code
const std::filesystem::path modelsDirectory{"./res/models"};

namespace Creepy{

    //TODO: Move to utils
    // Assimp: Row Major -> GLM: Column Major
    glm::mat4 convertMatrix(const aiMatrix4x4& matrix){
        // We can do
        // return glm::transpose(glm::make_mat4(&matrix.a1));
        

        glm::mat4 temp{};
        temp[0][0] = matrix.a1;
        temp[0][1] = matrix.b1;
        temp[0][2] = matrix.c1;
        temp[0][3] = matrix.d1;

        temp[1][0] = matrix.a2;
        temp[1][1] = matrix.b2;
        temp[1][2] = matrix.c2;
        temp[1][3] = matrix.d2;

        temp[2][0] = matrix.a3;
        temp[2][1] = matrix.b3;
        temp[2][2] = matrix.c3;
        temp[2][3] = matrix.d3;

        temp[3][0] = matrix.a4;
        temp[3][1] = matrix.b4;
        temp[3][2] = matrix.c4;
        temp[3][3] = matrix.d4;
        return temp;
    }

    static constinit uint32_t s_globalEntityID{1};

    Model::Model(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue) {
        this->LoadModel(filePath, device, commandPool, queue);
    }

    void Model::Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets) {
        for(auto& mesh : m_meshes){
            mesh.Draw(commandBuffer, pipelineLayout, descriptorSets);
        }
    }


    void Model::Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets, FragmentPushConstantData fragmentPushConstantData) {
        for(auto& mesh : m_meshes){
            mesh.Draw(commandBuffer, pipelineLayout, descriptorSets, this->getTransformMatrix(), fragmentPushConstantData);
        }
    }

    void Model::Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, FragmentPushConstantData fragmentPushConstantData) {
        for(auto& mesh : m_meshes){
            mesh.Draw(commandBuffer, pipelineLayout, this->getTransformMatrix(), fragmentPushConstantData);
        }
    }
            
    void Model::Destroy(const vk::Device device) const {
        for(auto&& mesh : m_meshes){
            mesh.Destroy(device);
        }
    }

    glm::vec3& Model::GetPosition() {
        return m_position;
    }

    glm::vec3& Model::GetRotation() {
        return m_rotation;
    }

    glm::vec3& Model::GetScale() {
        return m_scale;
    }

    std::span<const Mesh> Model::GetMeshes() const {
        return m_meshes;
    }

    std::span<Mesh> Model::GetMeshes() {
        return m_meshes;
    }

    

    void Model::LoadModel(const std::filesystem::path& filePath, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){
        m_entityID = s_globalEntityID++;
        
        Assimp::Importer importer{};

        auto scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);

        if(!scene){
            std::println("Failed to load model: {}", filePath.string());
        }
        
        this->processNode(scene->mRootNode, scene, glm::identity<glm::mat4>(), device, commandPool, queue);

        std::println("Total Mesh: {}", m_meshes.size());

        std::println("Total Loaded En: {}", s_globalEntityID);
    }

    void Model::SetMaterialIndex(uint32_t materialIndex)
    {
        m_materialIndex = materialIndex;
    }

    uint32_t Model::GetMaterialIndex() const
    {
        return m_materialIndex;
    }

    glm::mat4 Model::getTransformMatrix() const {
        return glm::translate(glm::identity<glm::mat4>(), m_position) * glm::toMat4(glm::quat{m_rotation}) * glm::scale(glm::identity<glm::mat4>(), m_scale);
    }

    void Model::processNode(aiNode* currentNode, const aiScene* currentScene, const glm::mat4& parentTransformMatrix, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){
        auto currentNodeTransformMatrix = convertMatrix(currentNode->mTransformation) * parentTransformMatrix;
        
        // Check Mesh Current Node
        for(uint32_t i{}; i < currentNode->mNumMeshes; ++i){
            auto currentMesh = currentScene->mMeshes[currentNode->mMeshes[i]];
            m_meshes.push_back(this->processMeshInterLeaved(currentMesh, currentScene, currentNodeTransformMatrix, device, commandPool, queue));
        }

        // Check Child Node
        for(uint32_t i{}; i < currentNode->mNumChildren; ++i){
            processNode(currentNode->mChildren[i], currentScene, currentNodeTransformMatrix, device, commandPool, queue);
        }
    }

    Mesh Model::processMeshInterLeaved(aiMesh* currentMesh, const aiScene* currentScene, const glm::mat4& parentTransformMatrix, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){
        std::vector<VertexInterLeave> vertices(currentMesh->mNumVertices);

        for(uint32_t i{}; i < currentMesh->mNumVertices; ++i){
            vertices[i].Position.x = currentMesh->mVertices[i].x;
            vertices[i].Position.y = currentMesh->mVertices[i].y;
            vertices[i].Position.z = currentMesh->mVertices[i].z;

            vertices[i].Normal.x = currentMesh->mNormals[i].x;
            vertices[i].Normal.y = currentMesh->mNormals[i].y;
            vertices[i].Normal.z = currentMesh->mNormals[i].z;
            
            
            if(currentMesh->HasTextureCoords(0)){
                vertices[i].TexCoord.x = currentMesh->mTextureCoords[0][i].x;
                vertices[i].TexCoord.y = currentMesh->mTextureCoords[0][i].y;
            }

            // Very Expensive
            vertices[i].EntityID = m_entityID;
        }

        std::vector<uint32_t> indices;

        for(uint32_t i{}; i < currentMesh->mNumFaces; ++i){
            
            auto currentFace = currentMesh->mFaces[i];

            for(uint32_t j{}; j < currentFace.mNumIndices; ++j){
                indices.emplace_back(currentFace.mIndices[j]);
            }

        }

        auto&& texturesPtr = this->loadMaterialTextures(currentScene->mMaterials[currentMesh->mMaterialIndex], currentScene, aiTextureType_DIFFUSE, device, commandPool, queue);
        return {device, commandPool, queue, vertices, indices, texturesPtr, parentTransformMatrix};
    }

    Mesh Model::processMeshSeparate(aiMesh* currentMesh, const aiScene* currentScene, const glm::mat4& parentTransformMatrix, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue) {
        VertexSeparate vertex{};
        
        if(currentMesh->HasPositions() && currentMesh->HasNormals() && currentMesh->HasTextureCoords(0)){
            vertex.Positions.reserve(currentMesh->mNumVertices);
            vertex.Normals.reserve(currentMesh->mNumVertices);
            vertex.TexCoords.reserve(currentMesh->mNumVertices);
            vertex.EntityID = m_entityID;
            
            for(uint32_t i{}; i < currentMesh->mNumVertices; ++i){
                vertex.Positions.emplace_back(currentMesh->mVertices[i].x, currentMesh->mVertices[i].y, currentMesh->mVertices[i].z);

                vertex.Normals.emplace_back(currentMesh->mNormals[i].x, currentMesh->mNormals[i].y, currentMesh->mNormals[i].z);

                vertex.TexCoords.emplace_back(currentMesh->mTextureCoords[0][i].x, currentMesh->mTextureCoords[0][i].y);
            }
        }
        else{
            std::println("Process Mesh Error!!!");
        }

        std::vector<uint32_t> indices;

        for(uint32_t i{}; i < currentMesh->mNumFaces; ++i){
            
            auto currentFace = currentMesh->mFaces[i];

            for(uint32_t j{}; j < currentFace.mNumIndices; ++j){
                indices.emplace_back(currentFace.mIndices[j]);
            }

        }

        auto&& texturesPtr = this->loadMaterialTextures(currentScene->mMaterials[currentMesh->mMaterialIndex], currentScene, aiTextureType_DIFFUSE, device, commandPool, queue);
        return {device, commandPool, queue, vertex, indices, texturesPtr, parentTransformMatrix};
    }

    std::vector<Texture*> Model::loadMaterialTextures(aiMaterial* currentMaterial, const aiScene* currentScene, aiTextureType textureType, const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue){
        std::vector<Texture*> textures;
        const uint32_t diffuseTextureCount{currentMaterial->GetTextureCount(textureType)};
        
        std::println("Total diffuse texture: {}", diffuseTextureCount);

        //TODO: If we have more texture then need enable it
        // textures.reserve(diffuseTextureCount);
        for(uint32_t i{}; i < diffuseTextureCount; ++i){
            aiString filePath{};
            auto res = currentMaterial->GetTexture(textureType, i, &filePath);
            
            if(res == aiReturn_SUCCESS){
                auto&& pathToTexture = modelsDirectory / filePath.C_Str();
                if(!TextureManager::IsContainTexture(pathToTexture)){
                    TextureManager::LoadTexture2D(pathToTexture, device, commandPool, queue);
                    std::println("Load Texture: {}", filePath.C_Str());
                }
                else{
                    std::println("Exit Texture: {}", filePath.C_Str());
                }

                textures.push_back(TextureManager::GetTexture<Texture>(pathToTexture));

            }
        }

        
        return textures;
    }

}