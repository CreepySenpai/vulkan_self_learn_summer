#pragma once

#include <vector>
#include "Vertex.hpp"
#include "Texture.hpp"
#include "VulkanBuffer.hpp"
#include <glm/glm.hpp>

namespace Creepy{

    struct Mesh{
        public:
            Mesh() = default;
            Mesh(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const Vertex> vertices, std::span<const uint32_t> indices, std::span<Texture*> textures, const glm::mat4& currentMeshTransform);
            

            void UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const Vertex> vertices, std::span<const uint32_t> indices) const;

            void Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets);
            void Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets, const glm::mat4& modelTransformMatrix, std::span<const vk::DeviceAddress> bufferAddresses);

            void Destroy(const vk::Device device) const;

            std::span<const Texture* const> GetTextures() const;

            std::span<Texture*> GetTextures();
        private:
            VertexBuffer m_vertexBuffer;
            IndexBuffer m_indexBuffer;
            std::vector<Texture*> m_textures;
            glm::mat4 m_currentMeshTransform;
    };

}

