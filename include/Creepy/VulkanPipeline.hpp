#pragma once

#include <vulkan/vulkan.hpp>

namespace Creepy{
    class PipelineState{
        public:
            void InitPipelineLayout(std::span<const vk::DescriptorSetLayout> descriptorSetLayouts, std::span<const vk::PushConstantRange> pushConstants);
            void InitShaderStates(vk::ShaderModule vertexShader, vk::ShaderModule fragmentShader);
            void InitVertexInputState(std::span<const vk::VertexInputBindingDescription> vertexBindings, std::span<const vk::VertexInputAttributeDescription> vertexAttributes);
            void InitInputAssemblyState(vk::PrimitiveTopology topology);
            void InitViewportState(uint32_t width, uint32_t height);
            void InitRasterizationState(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace);
            void InitMultiSamplerState();
            void InitDepthStencilState(vk::CompareOp depthCompareOp);
            void InitColorBlendState(std::span<const vk::PipelineColorBlendAttachmentState> colorBlendAttachments);
            void InitDynamicState(std::span<const vk::DynamicState> dynamicStates);
            void InitRenderingInfo(std::span<const vk::Format> colorAttachmentFormats, vk::Format depthAttachmentFormat);

            void Clear();
        public:
            void DisableMultiSampling();
            void DisableBlending();
            void DisableDepthTest();
            void EnableDepthTest();

        private:
            friend class Pipeline;
            std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStates;
            vk::PipelineLayoutCreateInfo m_pipelineLayoutInfo{};
            vk::PipelineVertexInputStateCreateInfo m_vertexInputState{};
            vk::PipelineInputAssemblyStateCreateInfo m_inputAssemblyState{};
            vk::PipelineViewportStateCreateInfo m_viewportState{};
            vk::PipelineRasterizationStateCreateInfo m_rasterizationState{};
            vk::PipelineMultisampleStateCreateInfo m_multisampleState{};
            vk::PipelineDepthStencilStateCreateInfo m_depthStencilState{};
            vk::PipelineColorBlendStateCreateInfo m_colorBlendState{};
            vk::PipelineDynamicStateCreateInfo m_dynamicState{};
            vk::PipelineRenderingCreateInfo m_renderingInfo{};
    };

    class Pipeline{
        public:
            void Build(const vk::Device device, const PipelineState& pipelineState);

            void Destroy(const vk::Device device) const;

            vk::Pipeline GetPipeline() const;
            vk::PipelineLayout GetPipelineLayout() const;
        private:
            vk::Pipeline m_pipeline;
            vk::PipelineLayout m_pipelineLayout;
    };
}