#pragma once

#include "VulkanBuffer.hpp"

namespace Creepy{

    class MaterialManager{
        public:
            static uint32_t AddMaterial(const vk::Device device);
            
            static MaterialData& GetMaterialData(uint32_t materialIndex);
            static vk::DeviceAddress GetBufferAddress(uint32_t materialIndex);

            static void UploadMaterialData();
            static void RemoveMaterial(uint32_t materialIndex);
            static void Destroy(const vk::Device device);

        private:
            static inline std::vector<MaterialBuffer> s_buffers;
            static inline std::vector<vk::DeviceAddress> s_bufferAddresses;
            static inline std::vector<MaterialData> s_bufferData;
            static inline std::vector<uint32_t> s_removedIndexes;
    };
}