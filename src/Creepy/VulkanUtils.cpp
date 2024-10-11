#include <Creepy/VulkanUtils.hpp>
#include <print>
#include <stb/stb_image_write.hpp>

namespace Creepy {

    // Only Use For Image Have Same Size
    void CopyImageToImage(const vk::CommandBuffer commandBuffer, const vk::Image srcImage, const vk::Image dstImage, uint32_t width, uint32_t height){
        vk::ImageCopy copyInfo{};
        copyInfo.extent = vk::Extent3D{width, height, 1};
        copyInfo.dstOffset = vk::Offset3D{0, 0, 0};
        copyInfo.srcOffset = vk::Offset3D{0, 0, 0};
        copyInfo.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        copyInfo.srcSubresource.layerCount = 1;
        copyInfo.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        copyInfo.dstSubresource.layerCount = 1;
        commandBuffer.copyImage(srcImage, vk::ImageLayout::eTransferSrcOptimal,
         dstImage, vk::ImageLayout::eTransferDstOptimal, copyInfo);
    }

    constexpr int totalChannel{4};
    static constinit uint32_t imageCaptureID{};

    void SaveImageToFile(const void* data, const uint32_t width, const uint32_t height) {
        auto saveImageFile = std::format("./res/textures/captures/capture_n{}.png", imageCaptureID++);
        if(!stbi_write_png(saveImageFile.c_str(), width, height, totalChannel, data, 0)){
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