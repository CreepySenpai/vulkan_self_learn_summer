#include <Creepy/Mesh.hpp>

namespace Creepy{

    Mesh::Mesh(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const Vertex> vertices, std::span<const uint32_t> indices)
        : m_vertexBuffer{device, vertices.size() * sizeof(Vertex)}, m_indexBuffer{device, indices.size() * sizeof(uint32_t)}
    {
        this->UploadData(device, commandPool, queue, vertices, indices);
    }

    void Mesh::UploadData(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const Vertex> vertices, std::span<const uint32_t> indices) const {
        m_vertexBuffer.UploadData(device, commandPool, queue, vertices);

        m_indexBuffer.UploadData(device, commandPool, queue, indices);
    }
    
    void Mesh::Destroy(const vk::Device device) const {
        m_vertexBuffer.Destroy(device);
        m_indexBuffer.Destroy(device);
    }

}