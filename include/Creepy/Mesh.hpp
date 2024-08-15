#pragma once

#include <vector>
#include "Vertex.hpp"
#include "VulkanBuffer.hpp"

namespace Creepy{

    struct Mesh{
        public:
            Mesh() = default;
            Mesh(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const Vertex> vertices, std::span<const uint32_t> indices);
            
            void UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const Vertex> vertices, std::span<const uint32_t> indices) const;

            void Destroy(const vk::Device device) const;
        private:
            VertexBuffer m_vertexBuffer;
            IndexBuffer m_indexBuffer;
    };

}

