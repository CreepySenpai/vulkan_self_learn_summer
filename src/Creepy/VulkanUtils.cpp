#include <Creepy/VulkanUtils.hpp>
#include <print>
#include <stb/stb_image_write.hpp>

namespace Creepy {

    constexpr int totalChannel{4};

    void SaveImageToFile(const void* data, const uint32_t width, const uint32_t height) {
        if(!stbi_write_png("./res/textures/captures/suck.png", width, height, totalChannel, data, 0)){
            std::println("Failed to capture image");
        }
    }


    vk::PipelineColorBlendAttachmentState GetAlphaBlending() {
        vk::PipelineColorBlendAttachmentState blending{};
        blending.blendEnable = vk::True;
        blending.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        blending.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        blending.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        blending.colorBlendOp = vk::BlendOp::eAdd;
        blending.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        blending.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        blending.alphaBlendOp = vk::BlendOp::eAdd;

        return blending;
    }

    vk::PipelineColorBlendAttachmentState GetAdditiveBlending() {
        vk::PipelineColorBlendAttachmentState blending{};
        blending.blendEnable = vk::True;
        blending.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        blending.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        blending.dstColorBlendFactor = vk::BlendFactor::eOne;
        blending.colorBlendOp = vk::BlendOp::eAdd;
        blending.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        blending.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        blending.alphaBlendOp = vk::BlendOp::eAdd;

        return blending;
    }
}