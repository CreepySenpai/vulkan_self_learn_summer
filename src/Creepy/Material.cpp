#include <Creepy/Material.hpp>


namespace Creepy{

    uint32_t MaterialManager::AddMaterial(const vk::Device device) {
        // Note: We reuse lasted remove index
        if(!s_removedIndexes.empty()){
            auto lastIndex = s_removedIndexes.back();
            s_removedIndexes.pop_back();
            s_bufferData.at(lastIndex) = MaterialData{};
            return lastIndex;
        }

        auto& createdBuffer = s_buffers.emplace_back(device, sizeof(MaterialData));

        vk::BufferDeviceAddressInfo bufferAddressInfo{};
        bufferAddressInfo.buffer = createdBuffer.GetBuffer();

        s_bufferAddresses.emplace_back(device.getBufferAddress(bufferAddressInfo));
        s_bufferData.push_back({});

        return s_buffers.size() - 1;    // return current material index
    }

    MaterialData& MaterialManager::GetMaterialData(uint32_t materialIndex) {
        return s_bufferData.at(materialIndex);
    }

    vk::DeviceAddress MaterialManager::GetBufferAddress(uint32_t materialIndex) {
        return s_bufferAddresses.at(materialIndex);
    }

    void MaterialManager::UploadMaterialData() {
        for(size_t i{}; const auto& buffer : s_buffers){
            buffer.UploadData(s_bufferData.at(i));
            ++i;
        }
    }

    void MaterialManager::RemoveMaterial(uint32_t materialIndex) {
        s_removedIndexes.push_back(materialIndex);
    }

    void MaterialManager::Destroy(const vk::Device device) {
        for(auto& buffer : s_buffers){
            buffer.Destroy(device);
        }
    }
}