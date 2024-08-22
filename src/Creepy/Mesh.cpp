#include <Creepy/Mesh.hpp>

namespace Creepy{

    Mesh::Mesh(const vk::Device device, const vk::CommandPool commandPool, const vk::Queue queue, std::span<const Vertex> vertices, std::span<const uint32_t> indices, std::span<Texture> textures)
        : m_vertexBuffer{device, vertices.size() * sizeof(Vertex)}, m_indexBuffer{device, indices.size() * sizeof(uint32_t)},
          m_textures{textures.begin(), textures.end()}
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

        for(auto& texture : m_textures){
            texture.Destroy(device);
        }
    }

    std::span<const Texture> Mesh::GetTextures() const {
        return m_textures;
    }

    std::span<Texture> Mesh::GetTextures() {
        return m_textures;
    }

}