#include <Creepy/Material.hpp>


namespace Creepy{

    uint32_t MaterialManager::AddMaterial(const vk::Device device) {
        // Note: We reuse lasted remove index
        if(!m_removedIndexes.empty()){
            auto lastIndex = m_removedIndexes.back();
            m_removedIndexes.pop_back();
            m_bufferData.at(lastIndex) = MaterialData{};
            return lastIndex;
        }

        auto& createdBuffer = m_buffers.emplace_back(device, sizeof(MaterialData));

        vk::BufferDeviceAddressInfo bufferAddressInfo{};
        bufferAddressInfo.buffer = createdBuffer.GetBuffer();

        m_bufferAddresses.emplace_back(device.getBufferAddress(bufferAddressInfo));
        m_bufferData.push_back({});

        return m_buffers.size() - 1;    // return current material index
    }

    MaterialData& MaterialManager::GetMaterialData(uint32_t materialIndex) {
        return m_bufferData.at(materialIndex);
    }

    vk::DeviceAddress MaterialManager::GetBufferAddress(uint32_t materialIndex) const {
        return m_bufferAddresses.at(materialIndex);
    }

    void MaterialManager::UploadMaterialData() const {
        for(size_t i{}; const auto& buffer : m_buffers){
            buffer.UploadData(m_bufferData.at(i));
            ++i;
        }
    }

    void MaterialManager::RemoveMaterial(uint32_t materialIndex) {
        m_removedIndexes.push_back(materialIndex);
    }

    void MaterialManager::Destroy(const vk::Device device) {
        for(auto& buffer : m_buffers){
            buffer.Destroy(device);
        }
    }
}