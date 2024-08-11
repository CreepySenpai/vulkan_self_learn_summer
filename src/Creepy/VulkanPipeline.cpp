#include <print>
#include <Creepy/VulkanPipeline.hpp>

void PipelineState::InitPipelineLayout(std::span<const vk::DescriptorSetLayout> descriptorSetLayouts, std::span<const vk::PushConstantRange> pushConstants) {

    m_pipelineLayoutInfo.flags = vk::PipelineLayoutCreateFlags{};
    m_pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    m_pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    m_pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
    m_pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

}

void PipelineState::InitShaderStates(vk::ShaderModule vertexShader, vk::ShaderModule fragmentShader){
    vk::PipelineShaderStageCreateInfo shaderInfo{};
    shaderInfo.flags = vk::PipelineShaderStageCreateFlags{};
    shaderInfo.pName = "main";

    shaderInfo.module = vertexShader;
    shaderInfo.stage = vk::ShaderStageFlagBits::eVertex;

    m_shaderStates.push_back(shaderInfo);

    shaderInfo.module = fragmentShader;
    shaderInfo.stage = vk::ShaderStageFlagBits::eFragment;

    m_shaderStates.push_back(shaderInfo);

}

void PipelineState::InitVertexInputState(std::span<const vk::VertexInputBindingDescription> vertexBindings, std::span<const vk::VertexInputAttributeDescription> vertexAttributes){
    m_vertexInputState.flags = vk::PipelineVertexInputStateCreateFlags{};
    m_vertexInputState.pVertexAttributeDescriptions = nullptr;
    m_vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
    m_vertexInputState.pVertexBindingDescriptions = vertexBindings.data();
    m_vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
    m_vertexInputState.pVertexAttributeDescriptions = vertexAttributes.data();
}

void PipelineState::InitInputAssemblyState(vk::PrimitiveTopology topology) {
    m_inputAssemblyState.flags = vk::PipelineInputAssemblyStateCreateFlags{};
    m_inputAssemblyState.topology = topology;
    m_inputAssemblyState.primitiveRestartEnable = vk::False;
}

void PipelineState::InitViewportState(uint32_t width, uint32_t height) {
    const vk::Viewport viewPort{0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f};

    vk::Rect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;

    scissor.extent.width = width;
    scissor.extent.height = height;
    
    m_viewportState.flags = vk::PipelineViewportStateCreateFlags{};
    m_viewportState.viewportCount = 1;
    m_viewportState.pViewports = &viewPort;
    m_viewportState.scissorCount = 1;
    m_viewportState.pScissors = &scissor;
}

void PipelineState::InitRasterizationState(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace) {

    m_rasterizationState.flags = vk::PipelineRasterizationStateCreateFlags{};
    m_rasterizationState.cullMode = cullMode;
    m_rasterizationState.polygonMode = polygonMode;
    m_rasterizationState.lineWidth = 1.0f;
    m_rasterizationState.frontFace = frontFace;
}

void PipelineState::InitMultiSamplerState() {
    m_multisampleState.flags = vk::PipelineMultisampleStateCreateFlags{};
    m_multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    m_multisampleState.minSampleShading = 1.0f;
    m_multisampleState.alphaToCoverageEnable = vk::False;
    m_multisampleState.alphaToOneEnable = vk::False;
}

void PipelineState::InitDepthStencilState(vk::CompareOp depthCompareOp) {
    m_depthStencilState.flags = vk::PipelineDepthStencilStateCreateFlags{};
    m_depthStencilState.depthTestEnable = vk::True;
    m_depthStencilState.depthWriteEnable = vk::False;
    m_depthStencilState.depthCompareOp = depthCompareOp;
    m_depthStencilState.depthBoundsTestEnable = vk::False;
    m_depthStencilState.stencilTestEnable = vk::False;

    m_depthStencilState.minDepthBounds = 0.0f;
    m_depthStencilState.maxDepthBounds = 1.0f;
}

void PipelineState::InitColorBlendState() {
    vk::PipelineColorBlendAttachmentState colorAttachment{};
    colorAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorAttachment.blendEnable = vk::True;
    colorAttachment.srcColorBlendFactor = vk::BlendFactor::eOneMinusDstAlpha;
    colorAttachment.dstColorBlendFactor = vk::BlendFactor::eDstAlpha;
    
    colorAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;

    colorAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorAttachment.alphaBlendOp = vk::BlendOp::eAdd;


    m_colorBlendState.flags = vk::PipelineColorBlendStateCreateFlags{};
    m_colorBlendState.logicOpEnable = vk::False;
    m_colorBlendState.logicOp = vk::LogicOp::eCopy;
    m_colorBlendState.attachmentCount = 1;
    m_colorBlendState.pAttachments = &colorAttachment;
}

void PipelineState::InitDynamicState(std::span<const vk::DynamicState> dynamicStates) {
    
    // constexpr std::array dynamicStates{
    //     vk::DynamicState::eViewport, vk::DynamicState::eScissor
    // };

    m_dynamicState.flags = vk::PipelineDynamicStateCreateFlags{};
    m_dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    m_dynamicState.pDynamicStates = dynamicStates.data();
}

void PipelineState::InitRenderingInfo(std::span<const vk::Format> colorAttachmentFormats, vk::Format depthAttachmentFormat) {
    m_renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentFormats.size());
    m_renderingInfo.pColorAttachmentFormats = colorAttachmentFormats.data();
    m_renderingInfo.depthAttachmentFormat = depthAttachmentFormat;
}

void Pipeline::Build(const vk::Device device, const PipelineState& pipelineState) {

    auto pipelineLayoutRes = device.createPipelineLayout(pipelineState.m_pipelineLayoutInfo);

    if(pipelineLayoutRes.result != vk::Result::eSuccess){
        std::println("Failed Create PipelineLayout");
    }

    m_pipelineLayout = pipelineLayoutRes.value;

    vk::GraphicsPipelineCreateInfo info{};
    info.flags = vk::PipelineCreateFlags{};
    info.pNext = &pipelineState.m_renderingInfo;
    info.layout = m_pipelineLayout;
    info.stageCount = static_cast<uint32_t>(pipelineState.m_shaderStates.size());
    info.pStages = pipelineState.m_shaderStates.data();
    info.pVertexInputState = &pipelineState.m_vertexInputState;
    info.pInputAssemblyState = &pipelineState.m_inputAssemblyState;
    info.pViewportState = &pipelineState.m_viewportState;
    info.pRasterizationState = &pipelineState.m_rasterizationState;
    info.pMultisampleState = &pipelineState.m_multisampleState;
    info.pDepthStencilState = &pipelineState.m_depthStencilState;
    info.pColorBlendState = &pipelineState.m_colorBlendState;
    info.pDynamicState = &pipelineState.m_dynamicState;

    info.renderPass = nullptr;
    info.subpass = 0;

    auto res = device.createGraphicsPipeline(nullptr, info);

    if(res.result != vk::Result::eSuccess){
        std::println("Failed Create Graphic Pipeline");
    }

    m_pipeline = res.value;
}

void Pipeline::Destroy(const vk::Device device) {
    device.destroyPipelineLayout(m_pipelineLayout);
    device.destroyPipeline(m_pipeline);
}