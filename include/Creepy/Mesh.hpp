#pragma once

#include <vector>
#include "Vertex.hpp"
#include "Texture.hpp"
#include "VulkanBuffer.hpp"
#include "PushConstant.hpp"
#include <glm/glm.hpp>

namespace Creepy{

    using VertexBuffer = std::variant<InterLeaveVertexBuffer, SeparateVertexBuffer>;

    struct Mesh{
        public:
            Mesh() = default;
            Mesh(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const VertexInterLeave> vertices, std::span<const uint32_t> indices, std::span<Texture*> textures, const glm::mat4& currentMeshTransform);
            Mesh(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, const VertexSeparate& vertex, std::span<const uint32_t> indices, std::span<Texture*> textures, const glm::mat4& currentMeshTransform);

            void UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const VertexInterLeave> vertices, std::span<const uint32_t> indices) const;

            void Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets);
            void Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets, const glm::mat4& modelTransformMatrix);

            void Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, std::span<const vk::DescriptorSet> descriptorSets, const glm::mat4& modelTransformMatrix, FragmentPushConstantData fragmentPushConstantData);

            void Draw(const vk::CommandBuffer commandBuffer, const vk::PipelineLayout pipelineLayout, const glm::mat4& modelTransformMatrix, FragmentPushConstantData fragmentPushConstantData);

            void Destroy(const vk::Device device) const;

            std::span<const Texture* const> GetTextures() const;

            std::span<Texture*> GetTextures();
        private:
            InterLeaveVertexBuffer m_vertexBuffer;
            VertexBuffer m_vertexBuffer2;
            IndexBuffer m_indexBuffer;
            std::vector<Texture*> m_textures;
            glm::mat4 m_currentMeshTransform;
    };

}

