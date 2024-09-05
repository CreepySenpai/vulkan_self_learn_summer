#pragma once

#include "VulkanBuffer.hpp"

namespace Creepy{

    class MaterialManager{
        public:
            uint32_t AddMaterial(const vk::Device device);
            
            MaterialData& GetMaterialData(uint32_t materialIndex);
            vk::DeviceAddress GetBufferAddress(uint32_t materialIndex) const;

            void UploadMaterialData() const;
            void Destroy(const vk::Device device);
        private:
            std::vector<MaterialBuffer> m_buffers;
            std::vector<vk::DeviceAddress> m_bufferAddresses;
            std::vector<MaterialData> m_bufferData;
    };
}