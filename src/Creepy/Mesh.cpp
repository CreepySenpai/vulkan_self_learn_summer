#include <print>
#include <Creepy/Mesh.hpp>

namespace Creepy{

    Mesh::Mesh(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const Vertex> vertices, std::span<const uint32_t> indices, std::span<Texture*> textures, const glm::mat4& currentMeshTransform)
        : m_vertexBuffer{device, vertices.size() * sizeof(Vertex)}, m_indexBuffer{device, indices.size() * sizeof(uint32_t)},
          m_textures{textures.begin(), textures.end()},
          m_currentMeshTransform{currentMeshTransform}
    {
        this->UploadData(device, commandPool, queue, vertices, indices);
    }

    void Mesh::UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const Vertex> vertices, std::span<const uint32_t> indices) const {
        m_vertexBuffer.UploadData(device, commandPool, queue, vertices);

        m_indexBuffer.UploadData(device, commandPool, queue, indices);
    }

    static std::vector<vk::DescriptorSet> getTotalSets(std::span<const Texture* const> baseMeshTextures, std::span<const vk::DescriptorSet> addMeshTextures);

    void Mesh::Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets) {
        auto&& totalSets = getTotalSets(m_textures, descriptorSets);

        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, totalSets, nullptr);
        
        constexpr std::array<uint64_t, 1> offsets{0};
        commandBuffer.bindVertexBuffers(0, m_vertexBuffer.GetBuffer(), offsets);
        commandBuffer.bindIndexBuffer(m_indexBuffer.GetBuffer(), 0, vk::IndexType::eUint32);
        commandBuffer.drawIndexed(m_indexBuffer.GetBufferCount(), 1, 0, 0, 0);
    }

    void Mesh::Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets, const glm::mat4& modelTransformMatrix) {
        auto&& totalSets = getTotalSets(m_textures, descriptorSets);

        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, totalSets, nullptr);
        
        const glm::mat4 modelMatrix = modelTransformMatrix * m_currentMeshTransform;

        commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), glm::value_ptr(modelMatrix));

        constexpr std::array<uint64_t, 1> offsets{0};
        commandBuffer.bindVertexBuffers(0, m_vertexBuffer.GetBuffer(), offsets);
        commandBuffer.bindIndexBuffer(m_indexBuffer.GetBuffer(), 0, vk::IndexType::eUint32);
        commandBuffer.drawIndexed(m_indexBuffer.GetBufferCount(), 1, 0, 0, 0);
    }

    void Mesh::Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets, const glm::mat4& modelTransformMatrix, FragmentPushConstantData fragmentPushConstantData) {
        auto&& totalSets = getTotalSets(m_textures, descriptorSets);

        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, totalSets, nullptr);

        const glm::mat4 modelMatrix = modelTransformMatrix * m_currentMeshTransform;

        commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), glm::value_ptr(modelMatrix));
        commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eFragment, sizeof(glm::mat4), sizeof(FragmentPushConstantData), &fragmentPushConstantData);
        
        constexpr std::array<uint64_t, 1> offsets{0};
        commandBuffer.bindVertexBuffers(0, m_vertexBuffer.GetBuffer(), offsets);
        commandBuffer.bindIndexBuffer(m_indexBuffer.GetBuffer(), 0, vk::IndexType::eUint32);
        commandBuffer.drawIndexed(m_indexBuffer.GetBufferCount(), 1, 0, 0, 0);
    }

    void Mesh::Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, const glm::mat4& modelTransformMatrix, FragmentPushConstantData fragmentPushConstantData) {
        const glm::mat4 modelMatrix = modelTransformMatrix * m_currentMeshTransform;
        
        // Current We only have diffuse texture so it fine
        // TODO: Add More Texture
        // const uint32_t textureIDs[]{
        //     m_textures.at(0)->GetTextureIndex()
        // };

        fragmentPushConstantData.diffuseTextureIndex = m_textures.at(0)->GetTextureIndex();

        commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), glm::value_ptr(modelMatrix));
        commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eFragment, sizeof(glm::mat4), sizeof(FragmentPushConstantData), &fragmentPushConstantData);

        constexpr std::array<uint64_t, 1> offsets{0};
        commandBuffer.bindVertexBuffers(0, m_vertexBuffer.GetBuffer(), offsets);
        commandBuffer.bindIndexBuffer(m_indexBuffer.GetBuffer(), 0, vk::IndexType::eUint32);
        commandBuffer.drawIndexed(m_indexBuffer.GetBufferCount(), 1, 0, 0, 0);
    }
    
    void Mesh::Destroy(const vk::Device device) const {
        m_vertexBuffer.Destroy(device);
        m_indexBuffer.Destroy(device);
    }

    std::span<const Texture* const> Mesh::GetTextures() const {
        return m_textures;
    }

    std::span<Texture*> Mesh::GetTextures() {
        return m_textures;
    }

    std::vector<vk::DescriptorSet> getTotalSets(std::span<const Texture* const> baseMeshTextures, std::span<const vk::DescriptorSet> addMeshTextures){
        std::vector<vk::DescriptorSet> totalSets;
        totalSets.reserve(baseMeshTextures.size() + addMeshTextures.size());
        
        // First Set For Uniform Buffer
        for(auto&& desc : addMeshTextures){
            totalSets.emplace_back(desc);
        }

        for(const auto& texture : baseMeshTextures){
            totalSets.emplace_back(texture->GetDescriptorSet());
        }

        return totalSets;
    }


}